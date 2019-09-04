#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <db/table.h>
#include <std/bits.h>
#include <std/hash_set.h>

namespace core
{

template <class ktype, class vtype, uint32_t (*hash_func)(const char *,int,uint32_t) = murmur3_32>
struct hashstore : table<pair<uint32_t, implier<ktype, vtype> > >
{
	hashstore() {}
	hashstore(const char *filename, bool overwrite = false) : table<pair<uint32_t, implier<ktype, vtype> > >(filename, overwrite) {}
	~hashstore() {}

	using table<pair<uint32_t, implier<ktype, vtype> > >::count;
	using table<pair<uint32_t, implier<ktype, vtype> > >::read;
	using table<pair<uint32_t, implier<ktype, vtype> > >::write;
	
	using typename table<pair<uint32_t, implier<ktype, vtype> > >::type;
	using typename table<pair<uint32_t, implier<ktype, vtype> > >::iterator;
	using typename table<pair<uint32_t, implier<ktype, vtype> > >::const_iterator;
	using table<pair<uint32_t, implier<ktype, vtype> > >::size;
	using table<pair<uint32_t, implier<ktype, vtype> > >::begin;
	using table<pair<uint32_t, implier<ktype, vtype> > >::end;
	using table<pair<uint32_t, implier<ktype, vtype> > >::rbegin;
	using table<pair<uint32_t, implier<ktype, vtype> > >::rend;
	using table<pair<uint32_t, implier<ktype, vtype> > >::sub;

	uint32_t hash_it(ktype key) const
	{
		bits h;
		hash_data(h, key);
		return hash_func((const char*)h.data, h.size(), 0);
	}

	iterator insert(ktype key, vtype value)
	{
		uint32_t hash = hash_it(key);
		iterator pos = lower_bound(begin(), end(), pair<uint32_t, ktype>(hash, key), 2, size());
		pos.push(pair<uint32_t, implier<ktype, vtype> >(hash, implier<ktype, vtype>(key, value)));
		return (pos-1);
	}

	iterator find(ktype key)
	{
		uint32_t hash = hash_it(key);
		iterator pos = lower_bound(begin(), end(), pair<uint32_t, ktype>(hash, key), 2, size());
		if (pos != end() && pos.get().second.key == key)
			return pos;
		else
			return end();
	}

	iterator at(ktype key)
	{
		uint32_t hash = hash_it(key);
		iterator pos = lower_bound(begin(), end(), pair<uint32_t, ktype>(hash, key), 2, size());
		if (pos == end() || pos.get().second.key != key) {
			pos.push(pair<uint32_t, implier<ktype, vtype> >(hash, implier<ktype, vtype>(key, vtype())));
			return pos-1;
		} else
			return pos;
	}

	void update(ktype key, vtype value, vtype (*U)(vtype, vtype))
	{
		uint32_t hash = hash_it(key);
		iterator pos = lower_bound(begin(), end(), pair<uint32_t, ktype>(hash, key), 2, size());
		if (!pos || pos.get().second.key != key) {
			pos.push(pair<uint32_t, implier<ktype, vtype> >(hash, implier<ktype, vtype>(key, value)));
		} else {
			pos.set(pair<uint32_t, implier<ktype, vtype> >(hash, implier<ktype, vtype>(pos.get().second.key, U(pos.get().second.value, value))));
		}
	}

	template <class container>
	void append(container &c, vtype (*U)(vtype, vtype))
	{
		if (c.size() == 0)
			return;

		typename container::iterator ci;
		uint64_t i, j;
		pair<uint32_t, implier<ktype, vtype> > x;

		i = count-1;
		ci = c.rbegin();
		if (i < count)
		{
			x = read(i--);
			while (ci != c.rend())
			{
				if (*(ci.ref) < x) {
					if (i < count)
						x = read(i--);
					else
						break;
				} else if (*(ci.ref) == x) {
					write(i+1, pair<uint32_t, implier<ktype, vtype> >(x.first, implier<ktype, vtype>(x.second.key, U(x.second.value, ci->value))));
					ci.drop();
					ci--;
					if (i >= 0)
						x = read(i--);
					else
						break;
				} else {
					ci--;
				}
			}

			i = count-1;
			ci = c.rbegin(); 
		}
		
		count += c.size();
		j = count-1;

		if (i < count - c.size())
		{
			x = read(i--);
			while (ci != c.rend())
			{
				if (*(ci.ref) < x) {
					write(j--, x);
					if (i < count - c.size())
						x = read(i--);
					else
						break;
				} else {
					write(j--, *(ci.ref));
					ci--;
				}
			}
		}

		while (ci != c.rend() && j < count)
		{
			write(j--, *(ci.ref));
			ci--;
		}

		c.clear();
	}
};

}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <db/table.h>

namespace file
{

template <class ktype, class vtype>
struct map : table<implier<ktype, vtype> >
{
	map() {}
	map(const char *filename, bool overwrite = false) : table<implier<ktype, vtype> >(filename, overwrite) {}
	~map() {}


	using table<implier<ktype, vtype> >::count;
	using table<implier<ktype, vtype> >::read;
	using table<implier<ktype, vtype> >::write;
	
	using typename table<implier<ktype, vtype> >::type;
	using typename table<implier<ktype, vtype> >::iterator;
	using typename table<implier<ktype, vtype> >::const_iterator;
	using table<implier<ktype, vtype> >::size;
	using table<implier<ktype, vtype> >::begin;
	using table<implier<ktype, vtype> >::end;
	using table<implier<ktype, vtype> >::rbegin;
	using table<implier<ktype, vtype> >::rend;
	using table<implier<ktype, vtype> >::sub;

	iterator insert(ktype key, vtype value)
	{
		iterator pos = lower_bound(begin(), end(), key, 2, size());
		pos.push(implier<ktype, vtype>(key, value));
		return (pos-1);
	}

	iterator find(ktype key)
	{
		iterator pos = lower_bound(begin(), end(), key, 2, size());
		if (pos != end() && pos.get().key == key)
			return pos;
		else
			return end();
	}

	iterator at(ktype key)
	{
		iterator pos = lower_bound(begin(), end(), key, 2, size());
		if (pos == end() || pos.get().key != key) {
			pos.push(implier<ktype, vtype>(key, vtype()));
			return pos-1;
		} else
			return pos;
	}

	void update(ktype key, vtype value, vtype (*U)(vtype, vtype))
	{
		iterator pos = lower_bound(begin(), end(), key, 2, size());
		if (!pos || pos.get().key != key) {
			pos.push(implier<ktype, vtype>(key, value));
		} else {
			pos.set(implier<ktype, vtype>(pos.get().key, U(pos.get().value, value)));
		}
	}

	template <class container>
	void append(container &c, vtype (*U)(vtype, vtype))
	{
		if (c.size() == 0)
			return;

		typename container::iterator ci;
		uint64_t i, j;
		implier<ktype, vtype> x;

		i = count-1;
		ci = c.rbegin();
		if (i < count)
		{
			x = read(i--);
			while (ci != c.rend())
			{
				if (*ci < x) {
					if (i < count)
						x = read(i--);
					else
						break;
				} else if (*ci == x) {
					write(i+1, implier<ktype, vtype>(x.key, U(x.value, ci->value)));
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
				if (*ci < x) {
					write(j--, x);
					if (i < count - c.size())
						x = read(i--);
					else
						break;
				} else {
					write(j--, *ci);
					ci--;
				}
			}
		}

		while (ci != c.rend() && j < count)
		{
			write(j--, *ci);
			ci--;
		}

		c.clear();
	}
};

}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <std/implier.h>
#include <std/slice.h>

namespace db
{

template <class value_type>
struct table
{
	typedef value_type type;

	struct const_iterator;
	
	struct iterator
	{
	protected:
		friend class table<value_type>;
		friend class const_iterator;

		table<value_type> *root;
		uint64_t index;

		value_type stored;
		value_type cached;
		uint64_t cached_index;
		bool modified;

		iterator(table<value_type> *root, int index)
		{
			this->root = root;
			this->index = index;
			this->cached_index = -1;
			modified = false;
		}

	public:
		typedef value_type type;

		iterator()
		{
			this->root = NULL;
			this->index = 0;
			this->cached_index = -1;
			modified = false;
		}

		~iterator()
		{
			save();
		}

		void load()
		{
			cached = root->read(index);
			stored = cached;
			cached_index = index;
			modified = false;
		}

		void save()
		{
			if (root != NULL &&
			    cached_index >= 0 &&
			    modified)
			{
				root->write(cached_index, cached);
				stored = cached;
				modified = false;
			}
		}

		void sync()
		{
			save();
			if (cached_index != index)
				load();
		}

		operator bool() const
		{
			return root != NULL && index >= 0 && index < root->count;
		}

		value_type operator*()
		{
			sync();
			return cached;
		}
		const value_type *operator->()
		{
			sync();
			return &cached;
		}

		const value_type *ptr()
		{
			sync();
			return &cached;
		}

		void set(value_type v)
		{
			cached = v;
			modified = true;
		}

		value_type get()
		{
			sync();
			return cached;
		}

		iterator &ref()
		{
			return *this;
		}

		const iterator &ref() const
		{
			return *this;
		}

		int idx() const
		{
			return index;
		}

		iterator &operator++(int)
		{
			index++;
			return *this;
		}

		iterator &operator--(int)
		{
			index--;
			return *this;
		}

		iterator &operator++()
		{
			++index;
			return *this;
		}

		iterator &operator--()
		{
			--index;
			return *this;
		}

		iterator &operator+=(int n)
		{
			index += n;
			return *this;
		}

		iterator &operator-=(int n)
		{
			index -= n;
			return *this;
		}

		iterator operator+(int n) const
		{
			return iterator(root, index+n);
		}

		iterator operator-(int n) const
		{
			return iterator(root, index-n);
		}

		bool operator==(iterator i) const
		{
			return index == i.index;
		}

		bool operator!=(iterator i) const
		{
			return index != i.index;
		}

		bool operator <(iterator i) const
		{
			return index < i.index;
		}

		bool operator >(iterator i) const
		{
			return index > i.index;
		}

		bool operator <=(iterator i) const
		{
			return index <= i.index;
		}

		bool operator >=(iterator i) const
		{
			return index >= i.index;
		}

		bool operator==(const_iterator i) const
		{
			return index == i.index;
		}

		bool operator!=(const_iterator i) const
		{
			return index != i.index;
		}

		bool operator <(const_iterator i) const
		{
			return index < i.index;
		}

		bool operator >(const_iterator i) const
		{
			return index > i.index;
		}

		bool operator <=(const_iterator i) const
		{
			return index <= i.index;
		}

		bool operator >=(const_iterator i) const
		{
			return index >= i.index;
		}

		int operator-(iterator i) const
		{
			return index - i.index;
		}

		int operator-(const_iterator i) const
		{
			return index - i.index;
		}

		slice<range<iterator> > sub(int length)
		{
			if (length < 0)
				return range<iterator>(*this+length, *this);
			else
				return range<iterator>(*this, *this+length);
		}

		slice<range<iterator> > sub()
		{
			return range<iterator>(*this, root->end());
		}

		void push(value_type v)
		{
			root->insert(index, v);
			index++;
			cached_index++;
		}

		iterator &operator=(iterator i)
		{
			save();
			root = i.root;
			index = i.index;
			if (i.cached_index == i.index)
			{
				cached = i.cached;
				stored = i.stored;
				cached_index = i.cached_index;
			}
			return *this;
		}
	};

	struct const_iterator
	{
	protected:
		friend class table<value_type>;
		friend class iterator;
		const table<value_type> *root;
		uint64_t index;

		value_type cached;
		uint64_t cached_index;

		const_iterator(const table<value_type> *root, int index)
		{
			this->root = root;
			this->index = index;
			this->cached_index = -1;
		}
	public:
		typedef value_type type;

		const_iterator()
		{
			root = NULL;
			index = 0;
			cached_index = -1;
		}

		const_iterator(iterator copy)
		{
			root = copy.root;
			index = copy.index;
			cached = copy.cached;
			cached_index = copy.cached_index;
		}

		~const_iterator() {}

		operator bool() const
		{
			return root != NULL && index >= 0 && index < root->count;
		}

		void read()
		{
			if (cached_index != index)
			{
				cached = root->read(index);
				cached_index = index;
			}
		}

		const value_type &operator*()
		{
			read();
			return cached;
		}

		const value_type *operator->()
		{
			read();
			return &cached;
		}

		const value_type *ptr()
		{
			read();
			return &cached;
		}

		const value_type &get()
		{
			read();
			return cached;
		}

		const_iterator &ref()
		{
			return *this;
		}

		const const_iterator &ref() const
		{
			return *this;
		}

		int idx() const
		{
			return index;
		}

		const_iterator &operator++(int)
		{
			index++;
			return *this;
		}

		const_iterator &operator--(int)
		{
			index--;
			return *this;
		}

		const_iterator &operator++()
		{
			++index;
			return *this;
		}

		const_iterator &operator--()
		{
			--index;
			return *this;
		}

		const_iterator &operator+=(int n)
		{
			index += n;
			return *this;
		}

		const_iterator &operator-=(int n)
		{
			index -= n;
			return *this;
		}

		const_iterator operator+(int n) const
		{
			return const_iterator(root, index+n);
		}

		const_iterator operator-(int n) const
		{
			return const_iterator(root, index-n);
		}

		bool operator==(const_iterator i) const
		{
			return index == i.index;
		}

		bool operator!=(const_iterator i) const
		{
			return index != i.index;
		}

		bool operator <(const_iterator i) const
		{
			return index < i.index;
		}

		bool operator >(const_iterator i) const
		{
			return index > i.index;
		}

		bool operator <=(const_iterator i) const
		{
			return index <= i.index;
		}

		bool operator >=(const_iterator i) const
		{
			return index >= i.index;
		}

		bool operator==(iterator i) const
		{
			return index == i.index;
		}

		bool operator!=(iterator i) const
		{
			return index != i.index;
		}

		bool operator <(iterator i) const
		{
			return index < i.index;
		}

		bool operator >(iterator i) const
		{
			return index > i.index;
		}

		bool operator <=(iterator i) const
		{
			return index <= i.index;
		}

		bool operator >=(iterator i) const
		{
			return index >= i.index;
		}

		int operator-(const_iterator i) const
		{
			return index - i.index;
		}

		int operator-(iterator i) const
		{
			return index - i.index;
		}

		slice<range<const_iterator> > sub(int length)
		{
			if (length < 0)
				return range<const_iterator>(*this+length, *this);
			else
				return range<const_iterator>(*this, *this+length);
		}

		slice<range<const_iterator> > sub()
		{
			return range<const_iterator>(*this, root->end());
		}

		const_iterator &operator=(const_iterator i)
		{
			root = i.root;
			index = i.index;
			cached = i.cached;
			cached_index = i.cached_index;
			return *this;
		}

		const_iterator &operator=(iterator i)
		{
			root = i.root;
			index = i.index;
			cached = i.cached;
			cached_index = i.cached_index;	
			return *this;
		}
	};

	table()
	{
		fptr = NULL;
		count = 0;
	}

	table(const char *filename, bool overwrite = false)
	{
		fptr = NULL;
		count = 0;
		open(filename, overwrite);
	}

	~table()
	{
		close();
	}

	FILE *fptr;
	uint64_t count;

	bool open(const char *filename, bool overwrite = false)
	{
		if (fptr != NULL)
			return false;

		if (overwrite)
			fptr = fopen(filename, "w+b");
		else {
			fptr = fopen(filename, "r+b");
			if (fptr == NULL)
				fptr = fopen(filename, "w+b");
			if (fptr == NULL)
				return false;
		}

		fseek(fptr, 0, SEEK_END);
		count = ftell(fptr)/sizeof(value_type);
		return true;
	}

	void close()
	{
		if (fptr != NULL)
			fclose(fptr);
		fptr = NULL;
		count = 0;
	}

	int size() const
	{
		return (int)count;
	}

	iterator begin()
	{
		return iterator(this, 0);
	}

	iterator end()
	{
		return iterator(this, count);
	}

	iterator rbegin()
	{
		return iterator(this, count-1);
	}

	iterator rend()
	{
		return iterator(this, -1);
	}

	const_iterator begin() const
	{
		return const_iterator(this, 0);
	}

	const_iterator end() const
	{
		return const_iterator(this, count);
	}

	const_iterator rbegin() const
	{
		return const_iterator(this, count-1);
	}

	const_iterator rend() const
	{
		return const_iterator(this, -1);
	}

	iterator at(int i)
	{
		return i < 0 ? end()+i : begin()+i;
	}

	const_iterator at(int i) const
	{
		return i < 0 ? end()+i : begin()+i;
	}

	value_type read(uint64_t index) const
	{
		value_type result;
		fseek(fptr, index*sizeof(value_type), SEEK_SET);
		fread(&result, sizeof(value_type), 1, fptr);
		return result;
	}

	void write(uint64_t index, const value_type &value)
	{
		fseek(fptr, index*sizeof(value_type), SEEK_SET);
		fwrite(&value, sizeof(value_type), 1, fptr);
		if (index >= count)
		{
			value_type init;
			fseek(fptr, count*sizeof(value_type), SEEK_SET);
			for (; count < index; count++)
				fwrite(&init, sizeof(value_type), 1, fptr);
			count++;
		}
	}

	uint64_t find(const value_type &value, value_type *found = NULL)
	{
		// do a binary search
		uint64_t lo = 0, hi = size();
		
		uint64_t pivot = lo + (hi - lo)/2;
		value_type data = read(pivot);
		while (hi != lo)
		{
			if (value < data)
				hi = pivot;
			else if (value > data)
				lo = pivot+1;
			else {
				lo = pivot;
				hi = pivot;
			}
			pivot = lo + (hi - lo)/2;
			data = read(pivot);
		}

		if (found != NULL)
			*found = data;

		return lo;
	}

	void insert(uint64_t index, const value_type &value)
	{
		value_type r[2];
		int i = 0;

		r[1-i] = value;

		fseek(fptr, index*sizeof(value_type), SEEK_SET);
		while (fread(r+i, sizeof(value_type), 1, fptr) > 0)
		{
			fseek(fptr, -sizeof(value_type), SEEK_CUR);
			fwrite(r+(1-i), sizeof(value_type), 1, fptr);
			i = 1-i;
		}
		fwrite(r+(1-i), sizeof(value_type), 1, fptr);
		
		if (index >= count)
		{
			value_type init;
			fseek(fptr, count*sizeof(value_type), SEEK_SET);
			for (; count < index; count++)
				fwrite(&init, sizeof(value_type), 1, fptr);
		}
		
		count++;
	}

	core::slice<range<iterator> > sub(int start, int end)
	{
		return range<iterator>(at(start), at(end));
	}

	core::slice<range<iterator> > sub(int start)
	{
		return range<iterator>(at(start), this->end());
	}

	core::slice<range<iterator> > sub()
	{
		return range<iterator>(begin(), end());
	}

	core::slice<range<const_iterator> > sub(int start, int end) const
	{
		return range<const_iterator>(at(start), at(end));
	}

	core::slice<range<const_iterator> > sub(int start) const
	{
		return range<const_iterator>(at(start), this->end());
	}

	core::slice<range<const_iterator> > sub() const
	{
		return range<const_iterator>(begin(), end());
	}

	static core::slice<range<iterator> > sub(iterator start, iterator end)
	{
		return range<iterator>(start, end).deref();
	}

	static core::slice<range<const_iterator> > sub(const_iterator start, const_iterator end)
	{
		return range<const_iterator>(start, end).deref();
	}
};

}

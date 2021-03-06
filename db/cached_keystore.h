#pragma once

#include "keystore.h"
#include "table.h"

#include <std/map.h>

#include <stdlib.h>
#include <stdio.h>

namespace core
{

template <typename key_type, typename value_type, int limit, value_type (*U)(value_type, value_type)>
struct cached_keystore
{
	cached_keystore() {}
	cached_keystore(const char *filename, bool overwrite = true) : store(filename, overwrite) { this->filename = filename; }
	~cached_keystore()
	{
		save();
	}

	typedef map<key_type, value_type> cache_type;
	typedef keystore<key_type, value_type> store_type;
	const char *filename;

	store_type store;
	cache_type cache;

	value_type *get(const key_type &key, FILE *log = stdout)
	{
		if (cache.size() >= limit)
			save(log);
		typename cache_type::iterator loc = cache.at(key);
		return &loc->value;
	}	

	void update(const key_type &key, const value_type &value, FILE *log = stdout)
	{
		cache.update(key, value, U);
		
		if (cache.size() >= limit)
			save(log);
	}

	void save(FILE *log = stdout)
	{
		//fprintf(log, "Saving %d items into %s[%d]...\t\t", cache.size(), filename, store.size());
		//fflush(log);
		store.append(cache, U);
		//fprintf(log, "Done %d[%d]\n", cache.size(), store.size());
	}
};

}

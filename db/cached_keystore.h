#pragma once

#include "keystore.h"
#include "table.h"

#include <std/hash_map.h>
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

	typedef hash_map<key_type, value_type> cache_type;
	typedef keystore<key_type, value_type> store_type;
	const char *filename;

	store_type store;
	cache_type cache;

	typename cache_type::iterator get(const key_type &key, FILE *log = stdout)
	{
		if (cache.size() >= limit)
			save(log);

		uint32_t hash = 0;
		implier<key_type, value_type> search(key, value_type());
		typename cache_type::item_iterator cloc = cache.position(search, &hash);
		if (!cloc or cloc->second.key != key) {
			typename store_type::iterator sloc = store.at(key);
			return cache.insert_at(cloc, sloc.get().second, &hash);
		} else {
			return typename cache_type::iterator(&cache, cloc);
		}
	}

	void update(const key_type &key, const value_type &value, FILE *log = stdout)
	{
		if (cache.size() >= limit)
			save(log);

		uint32_t hash = 0;
		implier<key_type, value_type> search(key, value);
		typename cache_type::item_iterator cloc = cache.position(search, &hash);
		if (!cloc or cloc->second.key != key) {
			cache.insert_at(cloc, search, &hash);
		} else {
			cloc->second.value = U(cloc->second.value, value);
		}
	}

	void save(FILE *log = stdout)
	{
		fprintf(log, "Saving %d items into %s[%d]...\t\t", cache.size(), filename, store.size());
		fflush(log);
		store.append(cache, U);
		fprintf(log, "Done %d[%d]\n", cache.size(), store.size());
	}
};

}

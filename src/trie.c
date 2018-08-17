/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2018 Massimo <dongmassimo@gmail.com>
 * 
 * soltant-uno is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * soltant-uno is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a Trie data structure to store name/value pairs,
 * where `name` is some string,
 * and `value` is a pointer of kind (void *)
 */

#include "soltant-uno.h"

/*
 * create a new Trie node
 * return:
 *   tr -- a new Trie node
 */
struct Trie *newTrie(){
	struct Trie *nt = malloc(sizeof(struct Trie));
	memset(nt,0,sizeof(struct Trie));
	return nt;
}

/*
 * destroy a Trie node and its children recursively
 * param:
 *   tr -- Trie node to destroy
 */
void delTrie(struct Trie *tr){
	for(int i=0;i<(1<<8);++i) if(tr->go[i]) delTrie(tr->go[i]);
	free(tr);
}

/*
 * add a name/value pair to a Trie
 * params:
 *   tr -- root of the Trie
 *   name -- name
 *   value -- value
 */
void trie_insert(struct Trie *tr, const char *name, void *value){
	for(int i=0;name[i]!='\0';++i){
		int d=name[i];
		if(tr->go[d] == NULL){
			tr->go[d] = newTrie();
		}

		tr = tr->go[d];
	}
	tr->value = value;
}

/*
 * get the value of a name from Trie
 * params:
 *   tr -- root of the Trie
 *   name -- name
 * return:
 *   the value corresponding to `name`
 */
void *trie_query(struct Trie *tr, const char *name){
	for(int i=0;name[i]!='\0';++i){
		int d=name[i];
		if(tr->go[d] == NULL){
			return NULL;
		}

		tr = tr->go[d];
	}
	return tr->value;
}

/*
 * enumerate all elements in trie
 * each execute foo
 */
void trie_enumerate(struct Trie *tr, void (*foo)(struct Trie *, void *), void *par){
	if(tr->value){
		foo(tr, par);
	}
	for(int d=0;d<TRIE_ALPBT_SIZE;++d) if(tr->go[d]){
		trie_enumerate(tr->go[d], foo, par);
	}
}
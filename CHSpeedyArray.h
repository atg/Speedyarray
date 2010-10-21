const int CHSA_COLLECTION_SIZE = 13;

#ifndef CHSA_ENUMERATE_BEGIN
#define CHSA_ENUMERATE_BEGIN(T, x_, xs_) CHSA< T >* exs_ = (xs_); int i_; for (i_ = 0; exs_ != NULL; i_++) { int ii_ = i_ % CHSA_COLLECTION_SIZE; T x_ = exs_->values[ii_];
#endif

#ifndef CHSA_ENUMERATE_END
#define CHSA_ENUMERATE_END if (ii_ == CHSA_COLLECTION_SIZE - 1) { exs_ = exs_->next; } }
#endif



template<typename T>
struct CHSA
{
	CHSA<T>* next;
	CHSA<T>* last;
	CHSA<T>* previous;
	T values[CHSA_COLLECTION_SIZE];
	unsigned char size;
};

template<typename T>
CHSA<T>* CHSA_Create()
{
	CHSA<T>* c = (CHSA<T>*)calloc(1, sizeof(CHSA<T>));
	c->last = c;
	return c;
}


//Attempt to create a slice from a provided cache
template<typename T>
CHSA<T>* CHSA_Create(CHSA<CHSA<T>* >* cache)
{
	if (cache->size == 0)
		return CHSA_Create<T>();
	
	CHSA<T>*c = CHSA_Pop(cache);
	if (c == NULL)
		return CHSA_Create<T>();
	
	memset(c, 0, sizeof(*c));
	c->last = c;
	
	return c;
}


template<typename T>
void CHSA_Add(CHSA<T>* collection, T value)
{
	CHSA<T>* c = collection->last;
		
	unsigned char s = c->size;
	if (__builtin_expect(s != CHSA_COLLECTION_SIZE, 1))
	{
		c->values[s] = value;
		c->size++;
	}
	else
	{
		CHSA<T>* newElement = CHSA_Create<T>();
		if (newElement == NULL)
			return;
		
		newElement->values[0] = value;
		
		collection->last = newElement;
		c->next = newElement;
		newElement->previous = c;
		newElement->size = 1;
	}
}

template<typename T>
void CHSA_Add(CHSA<T>* collection, T value, CHSA<CHSA<T>* >* cache)
{
	CHSA<T>* c = collection->last;//collection->last == NULL ? collection : collection->last;
	
	unsigned char s = c->size;
	if (__builtin_expect(s != CHSA_COLLECTION_SIZE, 1))
	{
		c->values[s] = value;
		c->size++;
	}
	else
	{
		CHSA<T>* newElement = CHSA_Create<T>(cache);
		if (newElement == NULL)
			return;
		
		newElement->values[0] = value;
		
		collection->last = newElement;
		c->next = newElement;
		newElement->previous = c;
		newElement->size = 1;
	}
}

template<typename T>
T CHSA_Pop(CHSA<T>* collection, CHSA<CHSA<T>* >* cache = 0)
{
	CHSA<T>* c = collection->last;
	unsigned char s = c->size;
	
	if (__builtin_expect(s != 1, 1))
	{
		if (__builtin_expect(s != 0, 1))
		{
			//There is more than one element. Get the last one and decrement size
			c->size--;
			return c->values[c->size];
		}
		else
		{
			//There are no elements. Panic
			assert(0);
			return (T)0;
		}
	}
	else
	{
		//There is only one element. Return it and get rid of this page
		T el = c->values[0];
		if (c->previous)
		{
			collection->last = c->previous;
			c->previous->next = 0;
		}
		
		if (c != collection)
		{
			if (cache)
				CHSA_Reclaim(c, cache);
			else
				CHSA_Reclaim(c);
		}
		
		return el;
	}
}

template<typename T>
void CHSA_Reclaim(CHSA<T>* collection)
{
	free(collection);
}

template<typename T>
void CHSA_Reclaim(CHSA<T>* collection, CHSA<CHSA<T>*>* cache)
{
	CHSA_Add<CHSA<T>*>(cache, collection);
}

template<typename T>
void CHSA_Destroy(CHSA<T>* collection)
{
	CHSA_Reclaim<T>(collection);
	CHSA<T>* n = collection->next;
	if (n == NULL)
		return;
	CHSA_Destroy<T>(n);
}

template<typename T>
void CHSA_Destroy(CHSA<T>* collection, CHSA<CHSA<T>*>* cache)
{
	CHSA_Reclaim<T>(collection, cache);
	CHSA<T>* n = collection->next;
	if (n == NULL)
		return;
	CHSA_Destroy<T>(n, cache);
}

template<typename T>
void CHSA_Destroy_Cache(CHSA<CHSA<T>* >* cache)
{
	CHSA_ENUMERATE_BEGIN(CHSA<long>*, item, cache) {
		
		CHSA_Destroy<T>(item);
		
	} CHSA_ENUMERATE_END
	
	CHSA_Destroy<CHSA<T>*>(cache);
}

from elasticsearch import AsyncElasticsearch
from elasticsearch import Elasticsearch

ES_INDEX_TWITTER_POST = 'twitter_post'

_es = None
def get_es():
    global _es
    if _es == None:
        _es = AsyncElasticsearch(
            "http://localhost:9200",
            basic_auth=('elastic', 'elastic')
        )
    return _es

_es_sync = None
def get_es_sync():
    global _es_sync
    if _es_sync == None:
        _es_sync = Elasticsearch(
            "http://localhost:9200",
            basic_auth=('elastic', 'elastic')
        )
    return _es_sync
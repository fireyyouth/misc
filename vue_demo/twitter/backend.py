from typing import Annotated
from fastapi import FastAPI, HTTPException, Request, Response, Cookie
from fastapi.responses import HTMLResponse
from sse_starlette.sse import EventSourceResponse
from pydantic import BaseModel
import jwt
import uvicorn
import time
import redis.asyncio as redis
import json
from fastapi.staticfiles import StaticFiles
import hashlib
import random
import es


app = FastAPI()


@app.get('/preview_list')
async def preview_list():
    es_client = es.get_es()
    es_result = await es_client.search(index=es.ES_INDEX_TWITTER_POST)
    r = []
    for hit in es_result['hits']['hits']:
        source = hit['_source']
        source['text'] = source['text'][:200] + '...'
        r.append(source)
    return r

@app.get('/content/{hash}')
async def content(hash):
    es_client = es.get_es()
    return es_client.get(index=es.ES_INDEX_TWITTER_POST, id=hash)

app.mount("/", StaticFiles(directory="frontend/dist", html=True), name="frontend")

uvicorn.run(app, host="0.0.0.0", port=8000)

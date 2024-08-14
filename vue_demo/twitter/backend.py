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

app = FastAPI()

def build_db():
    r = {}
    with open('data.txt') as f:
        for line in f:
            line = line.strip()
            if line:
                r[hashlib.md5(line.encode()).hexdigest()] = line
    return r


def build_img_db() -> list[str]:
    with open('images.json') as f:
        return json.load(f)

data_db = build_db()
img_db = build_img_db()

@app.get('/preview_list')
async def preview_list():
    r = []
    for hash, content in data_db.items():
        if len(content) > 200:
            r.append({
                'hash': hash,
                'content': content[:200] + '...',
                'img': [random.choice(img_db) for _ in range(random.randint(1, 4))]
            })
        else:
            r.append({
                'hash': None,
                'content': content,
                'img': [random.choice(img_db) for _ in range(random.randint(1, 4))]
            })
    return r

@app.get('/content/{hash}')
async def content(hash):
    return data_db[hash]

app.mount("/", StaticFiles(directory="frontend/dist", html=True), name="frontend")

uvicorn.run(app, host="0.0.0.0", port=8000)

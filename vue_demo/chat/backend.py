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

app = FastAPI()

user_db = {
        'frank': 'password',
        'tom': 'password',
        'peter': 'password',
        'lucy': 'password'
}

token_config = {
    'secret': 'abcdefg',
    'timeout': 3600
}

def gen_token(username):
    return jwt.encode({"exp": int(time.time() + token_config['timeout']), 'username': username}, token_config['secret'], algorithm="HS256")

def verify_token(token):
    return jwt.decode(token, token_config['secret'], algorithms=["HS256"])

class Credential(BaseModel):
    username: str
    password: str

@app.post('/auth')
async def auth(credential: Credential, response: Response):
    if credential.username in user_db and user_db[credential.username] == credential.password:
        response.set_cookie(key='chat_token', value=gen_token(credential.username))
    else:
        raise HTTPException(status_code=403, detail="incorrect credential")

redis_client = redis.Redis()
TOPIC_PUBLIC = 'channel:chat_public'


class Message(BaseModel):
    content: str

@app.post('/send_message')
async def events(chat_token: Annotated[str, Cookie()], message: Message):
    token = verify_token(chat_token)
    user_message = {
        'username': token['username'],
        'content': message.content
    }
    await redis_client.publish(TOPIC_PUBLIC, json.dumps(user_message))
    print('sent message to redis', user_message)

@app.get('/message_events')
async def events(chat_token: Annotated[str, Cookie()]):
    verify_token(chat_token)
    async def message_events():
        async with redis_client.pubsub() as pubsub:
            await pubsub.subscribe(TOPIC_PUBLIC)
            print('subscribed')
            while True:
                message = await pubsub.get_message(ignore_subscribe_messages=True, timeout=None)
                print('get from redis', message)
                if message and 'data' in message:
                    yield {'data': message['data'].decode()}

    return EventSourceResponse(message_events())

app.mount("/", StaticFiles(directory="frontend/dist", html=True), name="frontend")

uvicorn.run(app, host="0.0.0.0", port=8000)

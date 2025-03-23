from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn
import redis.asyncio as redis

app = FastAPI()

client = redis.Redis()

async def room_members(room):
    cursor = 0
    prefix = 'chat:{}:'.format(room)
    keys = []

    while True:
        cursor, partial_keys = await client.scan(cursor, match=prefix + '*')
        keys.extend(partial_keys)
        if cursor == 0:  # End of iteration
            break
    return [k[len(prefix):] for k in keys]

class PostModel(BaseModel):
    id: str

@app.post('/api/{room}')
async def register(room, data: PostModel):
    key = 'chat:{}:{}'.format(room, data.id)
    await client.set(key, '', ex=5)
    return await room_members(room);


uvicorn.run(app, port=5000, log_level="debug")

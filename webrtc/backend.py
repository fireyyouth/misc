from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn

app = FastAPI()

id_list = set()

@app.get('/api/')
def index():
    return list(id_list)

class PostModel(BaseModel):
    id: str

@app.post('/api/')
def register(data: PostModel):
    id_list.add(data.id)
    return list(id_list)

@app.delete('/api/')
def register(data: PostModel):
    id_list.remove(data.id)
    return list(id_list)



uvicorn.run(app, port=5000, log_level="debug")

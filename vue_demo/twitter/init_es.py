import hashlib
import random
import es
import json
import asyncio

def build_data():
    img_list = []
    with open('images.json') as f:
        img_list = json.load(f)

    img_id = 0

    r = {}
    with open('data.txt') as f:
        for line in f:
            line = line.strip()
            if line:
                key = hashlib.md5(line.encode()).hexdigest()
                r[key] = {
                    'text': line,
                    'img': img_list[img_id: img_id + 3]
                }
                img_id += 3
    return r

async def init_es():
    es_client = es.get_es().options(ignore_status=[400,404])
    try:
        data = build_data()
        await es_client.indices.delete(index=es.ES_INDEX_TWITTER_POST)
        for key, value in data.items():
            await es_client.create(index=es.ES_INDEX_TWITTER_POST, id=key, body=value)
    finally:
        await es_client.close()

asyncio.run(init_es())
import gensim.downloader as api
import numpy as np
import json

model = api.load('word2vec-google-news-300')
def get_embedding(filename, model):
    words = filename.split('.')
    word_embeddings = [model[word] for word in words if word in model]
    if len(word_embeddings) > 0:
        return np.mean(word_embeddings, axis=0).tolist()
    else:
        return np.zeros(model.vector_size).tolist()
def get_embedding_for_filename(filename):
    return get_embedding(filename, model)

target_filenames = ["diffuse.png", "metalness.png", "normal.png", "roughness.png", "ao.png"]
target_embeddings = {filename: get_embedding(filename, model) for filename in target_filenames}

with open('./src/target_embeddings.json', 'w') as f:
    json.dump(target_embeddings, f)
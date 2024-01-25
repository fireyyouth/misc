# for http://poj.org/problem?id=2050

import re

input = iter(open('input.txt'))

doc_list = []
doc_num = int(next(input))
for _ in range(doc_num):
    doc = []
    while True:
        line = next(input).strip()
        if line == '**********':
            break
        doc.append(line)
    doc_list.append(doc)

query_list = []
query_num = int(next(input))
for _ in range(query_num):
    query_list.append(next(input).strip())

#print(doc_list)
#print(query_list)
#print('-------------------------------------------')

doc_words = {}
index = {}
doc_id = 0
for doc_content in doc_list:
    doc_words[doc_id] = {}
    line_id = 0
    for line in doc_content:
        for word in re.split(r'[^a-zA-Z]+', line):
            word = word.lower()
            doc_words[doc_id][word] = 1
            if word not in index:
                index[word] = {}
            if doc_id not in index[word]:
                index[word][doc_id] = []
            index[word][doc_id].append((line_id, line))
        line_id += 1
    doc_id += 1



#print('-------------------------------------------')

def append_hit_lines(result, term):
    global index
    if term in index:
        for doc_id, line_list in index[term].items():
            if doc_id not in result:
                result[doc_id] = {}
            for line_id, line_content in line_list:
                result[doc_id][line_id] = line_content

result_list = []
for query in query_list:
    query = query.split()
    if len(query) == 1:
        result = {}
        append_hit_lines(result, query[0])
        result_list.append(result)

    elif len(query) == 2:
        assert query[0] == 'NOT'
        term = query[1]
        result = {doc_id: 1 for doc_id in range(len(doc_list))}
        if term in index:
            for doc, _ in index[term].items():
                result.pop(doc)
        result = {doc_id: {-1: '\n'.join(doc_list[doc_id])} for doc_id, _ in result.items()}
        result_list.append(result)

    else:
        assert len(query) == 3
        op = query[1]
        if op == 'OR':
            result = {}
            append_hit_lines(result, query[0])
            append_hit_lines(result, query[2])
            result_list.append(result)
        else:
            assert op == 'AND'
            result = {}
            # print('trying AND query', query)
            if query[0] in index:
                for doc, _ in index[query[0]].items():
                    #print("trying AND query for", query[0], doc)
                    if query[2] in doc_words[doc]:
                        #print('AND query hit', query, doc)
                        if doc not in result:
                            result[doc] = {}
                        for line_id, line_content in index[query[0]][doc]:
                            result[doc][line_id] = line_content
                        assert query[2] in index
                        for line_id, line_content in index[query[2]][doc]:
                            result[doc][line_id] = line_content
            result_list.append(result)

for result in result_list:
    if len(result) == 0:
        print('Sorry, I found nothing.')
    else:
        result = [result[doc_id] for doc_id in sorted(result.keys())]
        for i in range(len(result)):
            x = result[i]
            for line_id in sorted(x.keys()):
                print(x[line_id]) 
            if i != len(result) - 1:
                print('----------')        
    print('==========')                  

                        






            


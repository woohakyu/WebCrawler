# 일별 회사의 재무재표 목록을 DartApi에서 받기
# 재무재표이면 rcp_no로 Web 읽기
# .

import json
import requests

with open('../conf/common.json') as f:
	data = json.load(f)

#print(data['dartauthkey'])


def connect_request(u):
	try:
		return requests.get(u)
	except:
		print(respones.status_code)
		return -1

url = "http://dart.fss.or.kr/api/search.json?auth=d58011d7473b0d0f7bf602538ed3f17918fed9c9&crp_cd=005930&end_dt=20171114"
res = connect_request(url)

json_data = res.json()
#json_data = json.dumps(res.text)

print(type(json_data))
lst = json_data['list']
print(type(lst))
lst_data = json.dumps(lst)

print(type(lst_data))
print(lst_data)
#print(type(json_data['list']), '   ', json_data['list'])
#lst = json_data['list']
for k in lst_data.keys():
	print(k)

#for k in json_data:
#	print(k.keys())

#print(json_data)
#print(type(json_data['list']['rcp_no']))
#print(json_data['list']['rcp_no'])
#print(type(res.text))
#print(res.text)
#print(res.status_code)

#jstr=json.dumps(res.text)
#print(type(jstr))
#print(data)
#print(data['err_code'])
#print(data)
#
#data=json.loads(jstr)

#print(type(data))
#print(type(data['err_code']))
#print(data['err_code'])

#json.JSONDecoder(res.text)
#json.JSONEncoder(res.text)

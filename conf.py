# 일별 회사의 재무재표 목록을 DartApi에서 받기
# 재무재표이면 rcp_no로 Web 읽기
# ..

import json
import requests

#with open('../conf/common.json') as f:
#	data = json.load(f)

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

nErrCode = json_data["err_code"]
sErrMsg = json_data["err_msg"]
nPageNo = json_data["page_no"]
nTotCount = json_data["total_count"]
nTotPage = json_data["total_page"]

nCount = 0
while(1) :
        jdic = json_data["list"][nCount]
        print(jdic)
        fCrpCls = jdic["crp_cls"]
        sCrpNm = jdic["crp_nm"]
        nCrpCd = jdic["crp_cd"]
        sRptNm = jdic["rpt_nm"]
        nRcpNo = jdic["rcp_no"]
        sFlrNm = jdic["flr_nm"]
        dtRcpDt = jdic["rcp_dt"]
        vRmk = jdic["rmk"]

        if sRptNm.find("분기보고서") != -1 :
                print("True: ", sRptNm)
        else :
                print("Fail: ", sRptNm)
        
        nCount+=1
        print("Count Loop:", nCount)
        if(nCount == nTotCount) : break


#print("\ncheck point ... 1 [", json_data, "]")
#print("\ncheck point ... 2 [", json_data["list"], "]")
#print("\ncheck point ... 3 [", json_data["list"][0], "]")
#print("\ncheck point ... 4 [", json_data["list"][0]["crp_nm"], "]")




#if json_data['err_code'] != "010" :
#        print("Dart Respone Message: Error( %s )", json_data['err_code'])


#json_dump = json.dumps(json_data['list'])
#json_data = json.loads(json_dump)

#print(json_data['crp_nm'])
#for lst in json_data.keys() :
#        print(lst)

#lst_data = json.dumps(lst)

#print(type(lst_data))
#lst_json = json.loads(lst_data)

#print(type(lst_json))
#print(lst_json)
#print(type(json_data['list']), '   ', json_data['list'])
#lst = json_data['list']
#for k in lst_data.keys():
#	print(k)

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

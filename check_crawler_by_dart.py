
import json
import requests

dicCommonConf={}

def get_common_conf():
	with open('common.json') as f:
		data = json.load(f)
	sDartApiUrl = data["dart_api_json"]
	sDartAuthKey = data["dart_auth_key"]
	sDartViewer = data["dart_pc_viewer"]
	dicCommonConf["UrlApi"] = sDartApiUrl
	dicCommonConf["UrlView"] = sDartViewer
	dicCommonConf["AuthKey"] = sDartAuthKey

def set_file_write(name, mode, contents):
	try:
		with open (name, mode) as f:
			f.write(contents)
	except:
		print("file [", name, "] write error")
		return -1

def connect_request(u):
	try:
		return requests.get(u)
	except:
		print(respones.status_code)
		return -1

get_common_conf()
url = dicCommonConf["UrlApi"] + dicCommonConf["AuthKey"] + "&crp_cd=005930&end_dt=20171114"
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
	fCrpCls = jdic["crp_cls"]
	sCrpNm = jdic["crp_nm"]
	nCrpCd = jdic["crp_cd"]
	sRptNm = jdic["rpt_nm"]
	nRcpNo = jdic["rcp_no"]
	sFlrNm = jdic["flr_nm"]
	dtRcpDt = jdic["rcp_dt"]
	vRmk = jdic["rmk"]

	if sRptNm.find("분기보고서") != -1 :
		sConnUrl = dicCommonConf["UrlView"] + nRcpNo
		res = connect_request( sConnUrl )
		file_name = "read_list_crawler_url_dart_"+nRcpNo+".html"
		set_file_write(file_name,"w",res.text)

	nCount+=1
	if(nCount == nTotCount) : break


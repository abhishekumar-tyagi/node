'use strict';var cov_2o4gsty0gq=function(){var path='internal/cluster/child.js',hash='c37b81919e75b40b2566f2d4844b24ca83d8407e',global=new Function('return this')(),gcv='__coverage__',coverageData={path:'internal/cluster/child.js',statementMap:{'0':{start:{line:2,column:15},end:{line:2,column:32}},'1':{start:{line:3,column:13},end:{line:3,column:28}},'2':{start:{line:4,column:21},end:{line:4,column:38}},'3':{start:{line:5,column:15},end:{line:5,column:49}},'4':{start:{line:6,column:33},end:{line:6,column:66}},'5':{start:{line:7,column:16},end:{line:7,column:34}},'6':{start:{line:8,column:16},end:{line:8,column:18}},'7':{start:{line:9,column:16},end:{line:9,column:18}},'8':{start:{line:10,column:13},end:{line:10,column:21}},'9':{start:{line:12,column:0},end:{line:12,column:25}},'10':{start:{line:14,column:0},end:{line:14,column:24}},'11':{start:{line:15,column:0},end:{line:15,column:25}},'12':{start:{line:16,column:0},end:{line:16,column:22}},'13':{start:{line:17,column:0},end:{line:17,column:24}},'14':{start:{line:19,column:0},end:{line:47,column:2}},'15':{start:{line:20,column:17},end:{line:24,column:4}},'16':{start:{line:26,column:2},end:{line:26,column:26}},'17':{start:{line:28,column:2},end:{line:36,column:5}},'18':{start:{line:29,column:4},end:{line:29,column:30}},'19':{start:{line:31,column:4},end:{line:35,column:5}},'20':{start:{line:34,column:6},end:{line:34,column:22}},'21':{start:{line:38,column:2},end:{line:38,column:61}},'22':{start:{line:39,column:2},end:{line:39,column:26}},'23':{start:{line:42,column:4},end:{line:45,column:37}},'24':{start:{line:43,column:6},end:{line:43,column:36}},'25':{start:{line:44,column:9},end:{line:45,column:37}},'26':{start:{line:45,column:6},end:{line:45,column:37}},'27':{start:{line:50,column:0},end:{line:88,column:2}},'28':{start:{line:51,column:21},end:{line:54,column:44}},'29':{start:{line:56,column:2},end:{line:59,column:26}},'30':{start:{line:57,column:4},end:{line:57,column:28}},'31':{start:{line:59,column:4},end:{line:59,column:26}},'32':{start:{line:61,column:18},end:{line:65,column:13}},'33':{start:{line:68,column:2},end:{line:69,column:40}},'34':{start:{line:69,column:4},end:{line:69,column:40}},'35':{start:{line:71,column:2},end:{line:79,column:5}},'36':{start:{line:72,column:4},end:{line:73,column:37}},'37':{start:{line:73,column:6},end:{line:73,column:37}},'38':{start:{line:75,column:4},end:{line:78,column:32}},'39':{start:{line:76,column:6},end:{line:76,column:44}},'40':{start:{line:78,column:6},end:{line:78,column:32}},'41':{start:{line:81,column:2},end:{line:87,column:5}},'42':{start:{line:82,column:4},end:{line:82,column:39}},'43':{start:{line:83,column:20},end:{line:83,column:33}},'44':{start:{line:84,column:4},end:{line:84,column:30}},'45':{start:{line:85,column:4},end:{line:85,column:59}},'46':{start:{line:86,column:4},end:{line:86,column:18}},'47':{start:{line:92,column:14},end:{line:92,column:25}},'48':{start:{line:95,column:16},end:{line:95,column:28}},'49':{start:{line:97,column:2},end:{line:102,column:17}},'50':{start:{line:98,column:4},end:{line:98,column:32}},'51':{start:{line:99,column:4},end:{line:99,column:24}},'52':{start:{line:100,column:4},end:{line:100,column:31}},'53':{start:{line:101,column:4},end:{line:101,column:40}},'54':{start:{line:103,column:2},end:{line:103,column:37}},'55':{start:{line:104,column:2},end:{line:104,column:24}},'56':{start:{line:105,column:2},end:{line:105,column:28}},'57':{start:{line:110,column:2},end:{line:111,column:35}},'58':{start:{line:111,column:4},end:{line:111,column:35}},'59':{start:{line:113,column:12},end:{line:113,column:23}},'60':{start:{line:119,column:4},end:{line:119,column:13}},'61':{start:{line:128,column:4},end:{line:129,column:13}},'62':{start:{line:129,column:6},end:{line:129,column:13}},'63':{start:{line:131,column:4},end:{line:131,column:32}},'64':{start:{line:132,column:4},end:{line:132,column:24}},'65':{start:{line:133,column:4},end:{line:133,column:31}},'66':{start:{line:134,column:4},end:{line:134,column:20}},'67':{start:{line:138,column:4},end:{line:139,column:42}},'68':{start:{line:139,column:6},end:{line:139,column:42}},'69':{start:{line:141,column:4},end:{line:141,column:13}},'70':{start:{line:148,column:17},end:{line:148,column:58}},'71':{start:{line:150,column:2},end:{line:152,column:3}},'72':{start:{line:151,column:4},end:{line:151,column:37}},'73':{start:{line:154,column:2},end:{line:154,column:37}},'74':{start:{line:155,column:2},end:{line:155,column:24}},'75':{start:{line:156,column:2},end:{line:156,column:16}},'76':{start:{line:161,column:14},end:{line:161,column:25}},'77':{start:{line:162,column:17},end:{line:162,column:29}},'78':{start:{line:163,column:19},end:{line:163,column:39}},'79':{start:{line:165,column:2},end:{line:165,column:39}},'80':{start:{line:167,column:2},end:{line:168,column:35}},'81':{start:{line:168,column:4},end:{line:168,column:35}},'82':{start:{line:172,column:2},end:{line:172,column:48}},'83':{start:{line:176,column:2},end:{line:176,column:36}},'84':{start:{line:177,column:21},end:{line:177,column:22}},'85':{start:{line:180,column:4},end:{line:180,column:19}},'86':{start:{line:182,column:4},end:{line:192,column:5}},'87':{start:{line:187,column:6},end:{line:191,column:7}},'88':{start:{line:188,column:8},end:{line:188,column:29}},'89':{start:{line:190,column:8},end:{line:190,column:75}},'90':{start:{line:190,column:53},end:{line:190,column:73}},'91':{start:{line:195,column:2},end:{line:204,column:3}},'92':{start:{line:196,column:19},end:{line:196,column:31}},'93':{start:{line:197,column:4},end:{line:197,column:24}},'94':{start:{line:198,column:4},end:{line:198,column:19}},'95':{start:{line:200,column:4},end:{line:203,column:38}},'96':{start:{line:201,column:6},end:{line:201,column:44}},'97':{start:{line:203,column:6},end:{line:203,column:38}},'98':{start:{line:206,column:2},end:{line:206,column:22}},'99':{start:{line:210,column:0},end:{line:213,column:2}},'100':{start:{line:211,column:2},end:{line:211,column:25}},'101':{start:{line:212,column:2},end:{line:212,column:14}},'102':{start:{line:215,column:0},end:{line:224,column:2}},'103':{start:{line:216,column:2},end:{line:216,column:36}},'104':{start:{line:218,column:2},end:{line:223,column:3}},'105':{start:{line:219,column:4},end:{line:219,column:20}},'106':{start:{line:221,column:4},end:{line:221,column:71}},'107':{start:{line:221,column:49},end:{line:221,column:69}},'108':{start:{line:222,column:4},end:{line:222,column:54}},'109':{start:{line:222,column:37},end:{line:222,column:52}}},fnMap:{'0':{name:'(anonymous_0)',decl:{start:{line:10,column:13},end:{line:10,column:14}},loc:{start:{line:10,column:19},end:{line:10,column:21}},line:10},'1':{name:'(anonymous_1)',decl:{start:{line:19,column:23},end:{line:19,column:24}},loc:{start:{line:19,column:34},end:{line:47,column:1}},line:19},'2':{name:'(anonymous_2)',decl:{start:{line:28,column:29},end:{line:28,column:30}},loc:{start:{line:28,column:35},end:{line:36,column:3}},line:28},'3':{name:'onmessage',decl:{start:{line:41,column:11},end:{line:41,column:20}},loc:{start:{line:41,column:38},end:{line:46,column:3}},line:41},'4':{name:'(anonymous_4)',decl:{start:{line:50,column:21},end:{line:50,column:22}},loc:{start:{line:50,column:48},end:{line:88,column:1}},line:50},'5':{name:'(anonymous_5)',decl:{start:{line:71,column:16},end:{line:71,column:17}},loc:{start:{line:71,column:35},end:{line:79,column:3}},line:71},'6':{name:'(anonymous_6)',decl:{start:{line:81,column:24},end:{line:81,column:25}},loc:{start:{line:81,column:30},end:{line:87,column:3}},line:81},'7':{name:'shared',decl:{start:{line:91,column:9},end:{line:91,column:15}},loc:{start:{line:91,column:49},end:{line:106,column:1}},line:91},'8':{name:'(anonymous_8)',decl:{start:{line:97,column:17},end:{line:97,column:18}},loc:{start:{line:97,column:28},end:{line:102,column:3}},line:97},'9':{name:'rr',decl:{start:{line:109,column:9},end:{line:109,column:11}},loc:{start:{line:109,column:37},end:{line:157,column:1}},line:109},'10':{name:'listen',decl:{start:{line:115,column:11},end:{line:115,column:17}},loc:{start:{line:115,column:27},end:{line:120,column:3}},line:115},'11':{name:'close',decl:{start:{line:122,column:11},end:{line:122,column:16}},loc:{start:{line:122,column:19},end:{line:135,column:3}},line:122},'12':{name:'getsockname',decl:{start:{line:137,column:11},end:{line:137,column:22}},loc:{start:{line:137,column:28},end:{line:142,column:3}},line:137},'13':{name:'onconnection',decl:{start:{line:160,column:9},end:{line:160,column:21}},loc:{start:{line:160,column:39},end:{line:169,column:1}},line:160},'14':{name:'send',decl:{start:{line:171,column:9},end:{line:171,column:13}},loc:{start:{line:171,column:27},end:{line:173,column:1}},line:171},'15':{name:'_disconnect',decl:{start:{line:175,column:9},end:{line:175,column:20}},loc:{start:{line:175,column:38},end:{line:207,column:1}},line:175},'16':{name:'checkWaitingCount',decl:{start:{line:179,column:11},end:{line:179,column:28}},loc:{start:{line:179,column:31},end:{line:193,column:3}},line:179},'17':{name:'(anonymous_17)',decl:{start:{line:190,column:47},end:{line:190,column:48}},loc:{start:{line:190,column:53},end:{line:190,column:73}},line:190},'18':{name:'(anonymous_18)',decl:{start:{line:210,column:30},end:{line:210,column:31}},loc:{start:{line:210,column:41},end:{line:213,column:1}},line:210},'19':{name:'(anonymous_19)',decl:{start:{line:215,column:27},end:{line:215,column:28}},loc:{start:{line:215,column:38},end:{line:224,column:1}},line:215},'20':{name:'(anonymous_20)',decl:{start:{line:221,column:43},end:{line:221,column:44}},loc:{start:{line:221,column:49},end:{line:221,column:69}},line:221},'21':{name:'(anonymous_21)',decl:{start:{line:222,column:31},end:{line:222,column:32}},loc:{start:{line:222,column:37},end:{line:222,column:52}},line:222}},branchMap:{'0':{loc:{start:{line:31,column:4},end:{line:35,column:5}},type:'if',locations:[{start:{line:31,column:4},end:{line:35,column:5}},{start:{line:31,column:4},end:{line:35,column:5}}],line:31},'1':{loc:{start:{line:42,column:4},end:{line:45,column:37}},type:'if',locations:[{start:{line:42,column:4},end:{line:45,column:37}},{start:{line:42,column:4},end:{line:45,column:37}}],line:42},'2':{loc:{start:{line:44,column:9},end:{line:45,column:37}},type:'if',locations:[{start:{line:44,column:9},end:{line:45,column:37}},{start:{line:44,column:9},end:{line:45,column:37}}],line:44},'3':{loc:{start:{line:56,column:2},end:{line:59,column:26}},type:'if',locations:[{start:{line:56,column:2},end:{line:59,column:26}},{start:{line:56,column:2},end:{line:59,column:26}}],line:56},'4':{loc:{start:{line:68,column:2},end:{line:69,column:40}},type:'if',locations:[{start:{line:68,column:2},end:{line:69,column:40}},{start:{line:68,column:2},end:{line:69,column:40}}],line:68},'5':{loc:{start:{line:72,column:4},end:{line:73,column:37}},type:'if',locations:[{start:{line:72,column:4},end:{line:73,column:37}},{start:{line:72,column:4},end:{line:73,column:37}}],line:72},'6':{loc:{start:{line:75,column:4},end:{line:78,column:32}},type:'if',locations:[{start:{line:75,column:4},end:{line:78,column:32}},{start:{line:75,column:4},end:{line:78,column:32}}],line:75},'7':{loc:{start:{line:85,column:19},end:{line:85,column:58}},type:'binary-expr',locations:[{start:{line:85,column:19},end:{line:85,column:26}},{start:{line:85,column:30},end:{line:85,column:42}},{start:{line:85,column:46},end:{line:85,column:58}}],line:85},'8':{loc:{start:{line:110,column:2},end:{line:111,column:35}},type:'if',locations:[{start:{line:110,column:2},end:{line:111,column:35}},{start:{line:110,column:2},end:{line:111,column:35}}],line:110},'9':{loc:{start:{line:128,column:4},end:{line:129,column:13}},type:'if',locations:[{start:{line:128,column:4},end:{line:129,column:13}},{start:{line:128,column:4},end:{line:129,column:13}}],line:128},'10':{loc:{start:{line:138,column:4},end:{line:139,column:42}},type:'if',locations:[{start:{line:138,column:4},end:{line:139,column:42}},{start:{line:138,column:4},end:{line:139,column:42}}],line:138},'11':{loc:{start:{line:150,column:2},end:{line:152,column:3}},type:'if',locations:[{start:{line:150,column:2},end:{line:152,column:3}},{start:{line:150,column:2},end:{line:152,column:3}}],line:150},'12':{loc:{start:{line:167,column:2},end:{line:168,column:35}},type:'if',locations:[{start:{line:167,column:2},end:{line:168,column:35}},{start:{line:167,column:2},end:{line:168,column:35}}],line:167},'13':{loc:{start:{line:182,column:4},end:{line:192,column:5}},type:'if',locations:[{start:{line:182,column:4},end:{line:192,column:5}},{start:{line:182,column:4},end:{line:192,column:5}}],line:182},'14':{loc:{start:{line:187,column:6},end:{line:191,column:7}},type:'if',locations:[{start:{line:187,column:6},end:{line:191,column:7}},{start:{line:187,column:6},end:{line:191,column:7}}],line:187},'15':{loc:{start:{line:200,column:4},end:{line:203,column:38}},type:'if',locations:[{start:{line:200,column:4},end:{line:203,column:38}},{start:{line:200,column:4},end:{line:203,column:38}}],line:200},'16':{loc:{start:{line:218,column:2},end:{line:223,column:3}},type:'if',locations:[{start:{line:218,column:2},end:{line:223,column:3}},{start:{line:218,column:2},end:{line:223,column:3}}],line:218}},s:{'0':0,'1':0,'2':0,'3':0,'4':0,'5':0,'6':0,'7':0,'8':0,'9':0,'10':0,'11':0,'12':0,'13':0,'14':0,'15':0,'16':0,'17':0,'18':0,'19':0,'20':0,'21':0,'22':0,'23':0,'24':0,'25':0,'26':0,'27':0,'28':0,'29':0,'30':0,'31':0,'32':0,'33':0,'34':0,'35':0,'36':0,'37':0,'38':0,'39':0,'40':0,'41':0,'42':0,'43':0,'44':0,'45':0,'46':0,'47':0,'48':0,'49':0,'50':0,'51':0,'52':0,'53':0,'54':0,'55':0,'56':0,'57':0,'58':0,'59':0,'60':0,'61':0,'62':0,'63':0,'64':0,'65':0,'66':0,'67':0,'68':0,'69':0,'70':0,'71':0,'72':0,'73':0,'74':0,'75':0,'76':0,'77':0,'78':0,'79':0,'80':0,'81':0,'82':0,'83':0,'84':0,'85':0,'86':0,'87':0,'88':0,'89':0,'90':0,'91':0,'92':0,'93':0,'94':0,'95':0,'96':0,'97':0,'98':0,'99':0,'100':0,'101':0,'102':0,'103':0,'104':0,'105':0,'106':0,'107':0,'108':0,'109':0},f:{'0':0,'1':0,'2':0,'3':0,'4':0,'5':0,'6':0,'7':0,'8':0,'9':0,'10':0,'11':0,'12':0,'13':0,'14':0,'15':0,'16':0,'17':0,'18':0,'19':0,'20':0,'21':0},b:{'0':[0,0],'1':[0,0],'2':[0,0],'3':[0,0],'4':[0,0],'5':[0,0],'6':[0,0],'7':[0,0,0],'8':[0,0],'9':[0,0],'10':[0,0],'11':[0,0],'12':[0,0],'13':[0,0],'14':[0,0],'15':[0,0],'16':[0,0]},_coverageSchema:'332fd63041d2c1bcb487cc26dd0d5f7d97098a6c'},coverage=global[gcv]||(global[gcv]={});if(coverage[path]&&coverage[path].hash===hash){return coverage[path];}coverageData.hash=hash;return coverage[path]=coverageData;}();const assert=(cov_2o4gsty0gq.s[0]++,require('assert'));const util=(cov_2o4gsty0gq.s[1]++,require('util'));const EventEmitter=(cov_2o4gsty0gq.s[2]++,require('events'));const Worker=(cov_2o4gsty0gq.s[3]++,require('internal/cluster/worker'));const{internal,sendHelper}=(cov_2o4gsty0gq.s[4]++,require('internal/cluster/utils'));const cluster=(cov_2o4gsty0gq.s[5]++,new EventEmitter());const handles=(cov_2o4gsty0gq.s[6]++,{});const indexes=(cov_2o4gsty0gq.s[7]++,{});cov_2o4gsty0gq.s[8]++;const noop=()=>{cov_2o4gsty0gq.f[0]++;};cov_2o4gsty0gq.s[9]++;module.exports=cluster;cov_2o4gsty0gq.s[10]++;cluster.isWorker=true;cov_2o4gsty0gq.s[11]++;cluster.isMaster=false;cov_2o4gsty0gq.s[12]++;cluster.worker=null;cov_2o4gsty0gq.s[13]++;cluster.Worker=Worker;cov_2o4gsty0gq.s[14]++;cluster._setupWorker=function(){cov_2o4gsty0gq.f[1]++;const worker=(cov_2o4gsty0gq.s[15]++,new Worker({id:+process.env.NODE_UNIQUE_ID|0,process:process,state:'online'}));cov_2o4gsty0gq.s[16]++;cluster.worker=worker;cov_2o4gsty0gq.s[17]++;process.once('disconnect',()=>{cov_2o4gsty0gq.f[2]++;cov_2o4gsty0gq.s[18]++;worker.emit('disconnect');cov_2o4gsty0gq.s[19]++;if(!worker.exitedAfterDisconnect){cov_2o4gsty0gq.b[0][0]++;cov_2o4gsty0gq.s[20]++;// Unexpected disconnect, master exited, or some such nastiness, so
// worker exits immediately.
process.exit(0);}else{cov_2o4gsty0gq.b[0][1]++;}});cov_2o4gsty0gq.s[21]++;process.on('internalMessage',internal(worker,onmessage));cov_2o4gsty0gq.s[22]++;send({act:'online'});function onmessage(message,handle){cov_2o4gsty0gq.f[3]++;cov_2o4gsty0gq.s[23]++;if(message.act==='newconn'){cov_2o4gsty0gq.b[1][0]++;cov_2o4gsty0gq.s[24]++;onconnection(message,handle);}else{cov_2o4gsty0gq.b[1][1]++;cov_2o4gsty0gq.s[25]++;if(message.act==='disconnect'){cov_2o4gsty0gq.b[2][0]++;cov_2o4gsty0gq.s[26]++;_disconnect.call(worker,true);}else{cov_2o4gsty0gq.b[2][1]++;}}}};// obj is a net#Server or a dgram#Socket object.
cov_2o4gsty0gq.s[27]++;cluster._getServer=function(obj,options,cb){cov_2o4gsty0gq.f[4]++;const indexesKey=(cov_2o4gsty0gq.s[28]++,[options.address,options.port,options.addressType,options.fd].join(':'));cov_2o4gsty0gq.s[29]++;if(indexes[indexesKey]===undefined){cov_2o4gsty0gq.b[3][0]++;cov_2o4gsty0gq.s[30]++;indexes[indexesKey]=0;}else{cov_2o4gsty0gq.b[3][1]++;cov_2o4gsty0gq.s[31]++;indexes[indexesKey]++;}const message=(cov_2o4gsty0gq.s[32]++,util._extend({act:'queryServer',index:indexes[indexesKey],data:null},options));// Set custom data on handle (i.e. tls tickets key)
cov_2o4gsty0gq.s[33]++;if(obj._getServerData){cov_2o4gsty0gq.b[4][0]++;cov_2o4gsty0gq.s[34]++;message.data=obj._getServerData();}else{cov_2o4gsty0gq.b[4][1]++;}cov_2o4gsty0gq.s[35]++;send(message,(reply,handle)=>{cov_2o4gsty0gq.f[5]++;cov_2o4gsty0gq.s[36]++;if(typeof obj._setServerData==='function'){cov_2o4gsty0gq.b[5][0]++;cov_2o4gsty0gq.s[37]++;obj._setServerData(reply.data);}else{cov_2o4gsty0gq.b[5][1]++;}cov_2o4gsty0gq.s[38]++;if(handle){cov_2o4gsty0gq.b[6][0]++;cov_2o4gsty0gq.s[39]++;shared(reply,handle,indexesKey,cb);}// Shared listen socket.
else{cov_2o4gsty0gq.b[6][1]++;cov_2o4gsty0gq.s[40]++;rr(reply,indexesKey,cb);}// Round-robin.
});cov_2o4gsty0gq.s[41]++;obj.once('listening',()=>{cov_2o4gsty0gq.f[6]++;cov_2o4gsty0gq.s[42]++;cluster.worker.state='listening';const address=(cov_2o4gsty0gq.s[43]++,obj.address());cov_2o4gsty0gq.s[44]++;message.act='listening';cov_2o4gsty0gq.s[45]++;message.port=(cov_2o4gsty0gq.b[7][0]++,address)&&(cov_2o4gsty0gq.b[7][1]++,address.port)||(cov_2o4gsty0gq.b[7][2]++,options.port);cov_2o4gsty0gq.s[46]++;send(message);});};// Shared listen socket.
function shared(message,handle,indexesKey,cb){cov_2o4gsty0gq.f[7]++;const key=(cov_2o4gsty0gq.s[47]++,message.key);// Monkey-patch the close() method so we can keep track of when it's
// closed. Avoids resource leaks when the handle is short-lived.
const close=(cov_2o4gsty0gq.s[48]++,handle.close);cov_2o4gsty0gq.s[49]++;handle.close=function(){cov_2o4gsty0gq.f[8]++;cov_2o4gsty0gq.s[50]++;send({act:'close',key});cov_2o4gsty0gq.s[51]++;delete handles[key];cov_2o4gsty0gq.s[52]++;delete indexes[indexesKey];cov_2o4gsty0gq.s[53]++;return close.apply(this,arguments);}.bind(handle);cov_2o4gsty0gq.s[54]++;assert(handles[key]===undefined);cov_2o4gsty0gq.s[55]++;handles[key]=handle;cov_2o4gsty0gq.s[56]++;cb(message.errno,handle);}// Round-robin. Master distributes handles across workers.
function rr(message,indexesKey,cb){cov_2o4gsty0gq.f[9]++;cov_2o4gsty0gq.s[57]++;if(message.errno){cov_2o4gsty0gq.b[8][0]++;cov_2o4gsty0gq.s[58]++;return cb(message.errno,null);}else{cov_2o4gsty0gq.b[8][1]++;}var key=(cov_2o4gsty0gq.s[59]++,message.key);function listen(backlog){cov_2o4gsty0gq.f[10]++;cov_2o4gsty0gq.s[60]++;// TODO(bnoordhuis) Send a message to the master that tells it to
// update the backlog size. The actual backlog should probably be
// the largest requested size by any worker.
return 0;}function close(){cov_2o4gsty0gq.f[11]++;cov_2o4gsty0gq.s[61]++;// lib/net.js treats server._handle.close() as effectively synchronous.
// That means there is a time window between the call to close() and
// the ack by the master process in which we can still receive handles.
// onconnection() below handles that by sending those handles back to
// the master.
if(key===undefined){cov_2o4gsty0gq.b[9][0]++;cov_2o4gsty0gq.s[62]++;return;}else{cov_2o4gsty0gq.b[9][1]++;}cov_2o4gsty0gq.s[63]++;send({act:'close',key});cov_2o4gsty0gq.s[64]++;delete handles[key];cov_2o4gsty0gq.s[65]++;delete indexes[indexesKey];cov_2o4gsty0gq.s[66]++;key=undefined;}function getsockname(out){cov_2o4gsty0gq.f[12]++;cov_2o4gsty0gq.s[67]++;if(key){cov_2o4gsty0gq.b[10][0]++;cov_2o4gsty0gq.s[68]++;util._extend(out,message.sockname);}else{cov_2o4gsty0gq.b[10][1]++;}cov_2o4gsty0gq.s[69]++;return 0;}// Faux handle. Mimics a TCPWrap with just enough fidelity to get away
// with it. Fools net.Server into thinking that it's backed by a real
// handle. Use a noop function for ref() and unref() because the control
// channel is going to keep the worker alive anyway.
const handle=(cov_2o4gsty0gq.s[70]++,{close,listen,ref:noop,unref:noop});cov_2o4gsty0gq.s[71]++;if(message.sockname){cov_2o4gsty0gq.b[11][0]++;cov_2o4gsty0gq.s[72]++;handle.getsockname=getsockname;// TCP handles only.
}else{cov_2o4gsty0gq.b[11][1]++;}cov_2o4gsty0gq.s[73]++;assert(handles[key]===undefined);cov_2o4gsty0gq.s[74]++;handles[key]=handle;cov_2o4gsty0gq.s[75]++;cb(0,handle);}// Round-robin connection.
function onconnection(message,handle){cov_2o4gsty0gq.f[13]++;const key=(cov_2o4gsty0gq.s[76]++,message.key);const server=(cov_2o4gsty0gq.s[77]++,handles[key]);const accepted=(cov_2o4gsty0gq.s[78]++,server!==undefined);cov_2o4gsty0gq.s[79]++;send({ack:message.seq,accepted});cov_2o4gsty0gq.s[80]++;if(accepted){cov_2o4gsty0gq.b[12][0]++;cov_2o4gsty0gq.s[81]++;server.onconnection(0,handle);}else{cov_2o4gsty0gq.b[12][1]++;}}function send(message,cb){cov_2o4gsty0gq.f[14]++;cov_2o4gsty0gq.s[82]++;return sendHelper(process,message,null,cb);}function _disconnect(masterInitiated){cov_2o4gsty0gq.f[15]++;cov_2o4gsty0gq.s[83]++;this.exitedAfterDisconnect=true;let waitingCount=(cov_2o4gsty0gq.s[84]++,1);function checkWaitingCount(){cov_2o4gsty0gq.f[16]++;cov_2o4gsty0gq.s[85]++;waitingCount--;cov_2o4gsty0gq.s[86]++;if(waitingCount===0){cov_2o4gsty0gq.b[13][0]++;cov_2o4gsty0gq.s[87]++;// If disconnect is worker initiated, wait for ack to be sure
// exitedAfterDisconnect is properly set in the master, otherwise, if
// it's master initiated there's no need to send the
// exitedAfterDisconnect message
if(masterInitiated){cov_2o4gsty0gq.b[14][0]++;cov_2o4gsty0gq.s[88]++;process.disconnect();}else{cov_2o4gsty0gq.b[14][1]++;cov_2o4gsty0gq.s[89]++;send({act:'exitedAfterDisconnect'},()=>{cov_2o4gsty0gq.f[17]++;cov_2o4gsty0gq.s[90]++;return process.disconnect();});}}else{cov_2o4gsty0gq.b[13][1]++;}}cov_2o4gsty0gq.s[91]++;for(var key in handles){const handle=(cov_2o4gsty0gq.s[92]++,handles[key]);cov_2o4gsty0gq.s[93]++;delete handles[key];cov_2o4gsty0gq.s[94]++;waitingCount++;cov_2o4gsty0gq.s[95]++;if(handle.owner){cov_2o4gsty0gq.b[15][0]++;cov_2o4gsty0gq.s[96]++;handle.owner.close(checkWaitingCount);}else{cov_2o4gsty0gq.b[15][1]++;cov_2o4gsty0gq.s[97]++;handle.close(checkWaitingCount);}}cov_2o4gsty0gq.s[98]++;checkWaitingCount();}// Extend generic Worker with methods specific to worker processes.
cov_2o4gsty0gq.s[99]++;Worker.prototype.disconnect=function(){cov_2o4gsty0gq.f[18]++;cov_2o4gsty0gq.s[100]++;_disconnect.call(this);cov_2o4gsty0gq.s[101]++;return this;};cov_2o4gsty0gq.s[102]++;Worker.prototype.destroy=function(){cov_2o4gsty0gq.f[19]++;cov_2o4gsty0gq.s[103]++;this.exitedAfterDisconnect=true;cov_2o4gsty0gq.s[104]++;if(!this.isConnected()){cov_2o4gsty0gq.b[16][0]++;cov_2o4gsty0gq.s[105]++;process.exit(0);}else{cov_2o4gsty0gq.b[16][1]++;cov_2o4gsty0gq.s[106]++;send({act:'exitedAfterDisconnect'},()=>{cov_2o4gsty0gq.f[20]++;cov_2o4gsty0gq.s[107]++;return process.disconnect();});cov_2o4gsty0gq.s[108]++;process.once('disconnect',()=>{cov_2o4gsty0gq.f[21]++;cov_2o4gsty0gq.s[109]++;return process.exit(0);});}};
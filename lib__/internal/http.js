'use strict';var cov_o7wpyvvwj=function(){var path='internal/http.js',hash='2c1507fe48ac01ea0000ed693a919e595bd02b29',global=new Function('return this')(),gcv='__coverage__',coverageData={path:'internal/http.js',statementMap:{'0':{start:{line:3,column:15},end:{line:3,column:32}},'1':{start:{line:7,column:2},end:{line:12,column:3}},'2':{start:{line:8,column:14},end:{line:8,column:24}},'3':{start:{line:9,column:4},end:{line:9,column:32}},'4':{start:{line:10,column:4},end:{line:10,column:55}},'5':{start:{line:11,column:4},end:{line:11,column:33}},'6':{start:{line:13,column:2},end:{line:13,column:19}},'7':{start:{line:15,column:0},end:{line:17,column:2}},'8':{start:{line:16,column:2},end:{line:16,column:24}},'9':{start:{line:20,column:2},end:{line:20,column:57}},'10':{start:{line:20,column:25},end:{line:20,column:57}},'11':{start:{line:23,column:0},end:{line:27,column:2}}},fnMap:{'0':{name:'utcDate',decl:{start:{line:6,column:9},end:{line:6,column:16}},loc:{start:{line:6,column:19},end:{line:14,column:1}},line:6},'1':{name:'(anonymous_1)',decl:{start:{line:15,column:21},end:{line:15,column:22}},loc:{start:{line:15,column:32},end:{line:17,column:1}},line:15},'2':{name:'ondrain',decl:{start:{line:19,column:9},end:{line:19,column:16}},loc:{start:{line:19,column:19},end:{line:21,column:1}},line:19}},branchMap:{'0':{loc:{start:{line:7,column:2},end:{line:12,column:3}},type:'if',locations:[{start:{line:7,column:2},end:{line:12,column:3}},{start:{line:7,column:2},end:{line:12,column:3}}],line:7},'1':{loc:{start:{line:20,column:2},end:{line:20,column:57}},type:'if',locations:[{start:{line:20,column:2},end:{line:20,column:57}},{start:{line:20,column:2},end:{line:20,column:57}}],line:20}},s:{'0':0,'1':0,'2':0,'3':0,'4':0,'5':0,'6':0,'7':0,'8':0,'9':0,'10':0,'11':0},f:{'0':0,'1':0,'2':0},b:{'0':[0,0],'1':[0,0]},_coverageSchema:'332fd63041d2c1bcb487cc26dd0d5f7d97098a6c'},coverage=global[gcv]||(global[gcv]={});if(coverage[path]&&coverage[path].hash===hash){return coverage[path];}coverageData.hash=hash;return coverage[path]=coverageData;}();const timers=(cov_o7wpyvvwj.s[0]++,require('timers'));var dateCache;function utcDate(){cov_o7wpyvvwj.f[0]++;cov_o7wpyvvwj.s[1]++;if(!dateCache){cov_o7wpyvvwj.b[0][0]++;const d=(cov_o7wpyvvwj.s[2]++,new Date());cov_o7wpyvvwj.s[3]++;dateCache=d.toUTCString();cov_o7wpyvvwj.s[4]++;timers.enroll(utcDate,1000-d.getMilliseconds());cov_o7wpyvvwj.s[5]++;timers._unrefActive(utcDate);}else{cov_o7wpyvvwj.b[0][1]++;}cov_o7wpyvvwj.s[6]++;return dateCache;}cov_o7wpyvvwj.s[7]++;utcDate._onTimeout=function(){cov_o7wpyvvwj.f[1]++;cov_o7wpyvvwj.s[8]++;dateCache=undefined;};function ondrain(){cov_o7wpyvvwj.f[2]++;cov_o7wpyvvwj.s[9]++;if(this._httpMessage){cov_o7wpyvvwj.b[1][0]++;cov_o7wpyvvwj.s[10]++;this._httpMessage.emit('drain');}else{cov_o7wpyvvwj.b[1][1]++;}}cov_o7wpyvvwj.s[11]++;module.exports={outHeadersKey:Symbol('outHeadersKey'),ondrain,utcDate};
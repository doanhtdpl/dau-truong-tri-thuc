

var express = require('express');
var app = express.createServer();
app.use(express.logger());


app.post('/', function(req, res){
    res.send('Đấu trường tri thức');
});


app.post('/subject', function(req, res){
	var obj = {
		list:
		[
			{
				subjectId: 1,
				subjectName: "Khoa học tự nhiên"
			},
			{
				subjectId: 2,
				subjectName: "Lịch sử"
			},
			{
				subjectId: 3,
				subjectName: "Văn học"
			},
			{
				subjectId: 4,
				subjectName: "Nghệ thuật"
			},
			{
				subjectId: 5,
				subjectName: "Hiểu biết chung"
			},
			{
				subjectId: 6,
				subjectName: "Thể thao"
			},
			{
				subjectId: 7,
				subjectName: "Du lịch"
			},
			{
				subjectId: 8,
				subjectName: "Công nghệ thôn tin"
			},
			{
				subjectId: 9,
				subjectName: "Điện ảnh"
			}
		]
	};

    res.send(obj);
});


app.post('/join', function(req, res){
	var obj = {
		oppId: "123456789",
		oppName: "Bích Phương",
		oppWin: 210,
		oppLose: 90,
		quest: "Tổng thống Mỹ đầu tiên là ai?",
		a: "Osama Biladen",
		b: "Barack Obama",
		c: "George Washington",
		d: "John Kennedy",
		right: 2
	};

    res.send(obj);
});


app.post('/config', function(req, res){
	var obj = {
		WinScore: 10,
		LoseScore: -5,
		HelpSkip: 20,
		HelpExclusive: 10,
		HelpInfinite: 10
	};

    res.send(obj);
});



app.listen(8025, '127.0.0.1');
console.log('Express server started on port %s', 8025);



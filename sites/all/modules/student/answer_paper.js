$(function function_name () {

    var timeLimit = parseInt($('#timelimit').html());
    var timeLeft = timeLimit;
    var timeUsed = 0;

    $('#student-all').append('<input type="hidden" name="time_used" value="0"></input>');
    var interId = setInterval(function(){
        
        timeUsed += 1;
        timeLeft -= 1;

        $('#timelimit').html(timeLeft);
        $('input[name=time_used]').val(timeUsed);

        if(timeLeft <= 0) {
            submitPaper();
        }

    }, 60000);

    function submitPaper() {
        clearInterval(interId);
        alert('答题时间已耗尽，自动提交试卷。');
        $('#student-all').submit();
    }
});



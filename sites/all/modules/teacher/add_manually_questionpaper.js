$(function function_name(argument) {

    var questionList = {};

    //插入div用来装载隐藏input项
    $('#teacher-manually-questionpaper-form').append('<div id="add-input"></div>');

    //点击Pager进行ajax翻页
    $('#teacher-manually-questionpaper-form').delegate('.pager a', 'click', function() {

        var pagerUrl = $(this).attr('href');
        getListFromPage(pagerUrl, questionList);

        return false;
    });

    //点击checkbox，更新试题list
    $('#teacher-options-list').delegate('.form-checkbox .option input', 'click', function() {

        var questionId = parseInt($(this).attr('name'));

        if(typeof questionList[questionId] === 'undefined') {
            questionList[questionId] = {};
        }

        //更新试题list选中状态
        questionList[questionId]['checked'] = $(this).is(':checked');

        //获取点击试题分数
        var row = $(this).closest('tr');
        var score = row.children('td:nth-child(6)').html();
        score = parseInt(score);
        
        questionList[questionId]['score'] = score;


        //求总分
        var totalScore = sumScore(questionList);
        $('#total-score').html(totalScore);
    });

});

//ajax载入试题列表和pager
function getListFromPage(pagerUrl, questionList) {

    $('#teacher-options-list tbody').load(pagerUrl + ' #teacher-options-list tbody tr', function() {
        //翻页后根据questionList更新试题checkbox选中状态，并添加隐藏input
        updateFormWithQuestionList(questionList);
    });

    $('#teacher-manually-questionpaper-form .pager').load(pagerUrl + ' #teacher-manually-questionpaper-form .pager li');
    
}

//根据questionList更新试题checkbox选中状态，并添加隐藏input
function updateFormWithQuestionList(questionList) {

    $('#add-input').html('');
    $.each(questionList, function(index, question) {

        if (question.checked === true) {

            var dom = $('#edit-' + index + '-id');

            if(dom.length > 0) {
                //题目存在当前页, 更新选中状态
                dom.attr('checked', true);
            } else {
                //题目不在当前页, 添加隐藏题号input
                var input = '<input type="hidden" name="' + index + '" value="1">';
                $('#add-input').append(input);
            }
        } 

    });

}

/**
 * 求所选试题总分
 */
function sumScore(questionList) {

    var sum = 0;

    $.each(questionList, function(index, question) {
        if (question.checked === true) {
            sum += question.score;
        }
    });

    return sum;
}
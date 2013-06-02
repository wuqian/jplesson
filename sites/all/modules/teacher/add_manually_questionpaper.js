$(function function_name(argument) {

    var questionList = {};

    $('#teacher-manually-questionpaper-form').append('<div id="add-input"></div>')


    //点击Pager进行ajax翻页
    $('#teacher-manually-questionpaper-form').delegate('.pager a', 'click', function() {

        var pagerUrl = $(this).attr('href');

        getListFromPage(pagerUrl, questionList);

        return false;
    });

    //点击checkbox，更新试题列表
    $('#teacher-options-list').delegate('.form-checkbox .option input', 'click', function() {

        var questionId = parseInt($(this).attr('name'));

        if(typeof questionList[questionId] === 'undefined') {
            questionList[questionId] = {};
        }

        questionList[questionId]['checked'] = $(this).is(':checked');
        questionList[questionId]['score'] = 2;

        //求总分
        var totalScore = sumScore(questionList);
        $('#total-score').html(totalScore);
    });

});


function getListFromPage(pagerUrl, questionList) {

    $('#teacher-options-list tbody').load(pagerUrl + ' #teacher-options-list tbody tr', function() {
        //更新题目选定状态
        updateFormWithQuestionList(questionList);
    });

    $('#teacher-manually-questionpaper-form .pager').load(pagerUrl + ' #teacher-manually-questionpaper-form .pager li');
    
}

function updateFormWithQuestionList(questionList) {

    $('#add-input').html('');
    $.each(questionList, function(index, value) {

        if (value.checked === true) {

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


    $.each(questionList, function(index, value) {
        if (value.checked === true) {
            sum += 2;
        }
    });

    return sum;
}
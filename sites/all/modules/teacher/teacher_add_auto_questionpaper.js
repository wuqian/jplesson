$(document).ready(function(){
    var currentCourse = $('#edit-course').val();
    $('#' + currentCourse + '-setting').appendTo('#settingsInput');

    $('#edit-course').change(function(){
        //console.log(this.value);
        $('#settingsInput .course-settings').appendTo('#unusedInput');
        //获取课程名称
        var id = '#' + this.value + '-setting';
        $(id).appendTo('#settingsInput');
    });

    
    $('.types-settings input').blur(sumScore);

});

function sumScore() {
  
  var totalScore = 0;

  $('#settingsInput .types-settings input').each(function(index) {

    var score = parseInt($(this).val());

    if(isNaN(score)) {
      return;
    }

    if($(this).hasClass('text-type')) {
      //问答题计10分
      totalScore += 10 * score;
    } else {
      //其他题计2分
      totalScore += 2 * score;
    }
    
  });

  var totalScoreDom =  $('#total-score');
  totalScoreDom.text(totalScore);

}
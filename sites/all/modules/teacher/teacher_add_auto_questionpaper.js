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
});
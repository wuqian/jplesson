$(document).ready(function(){

    $('#chapters .chapter').hide();
    var currentCourse = $('#edit-course').val();
    $('#' + currentCourse + '-wrapper').show();

    $('#edit-course').change(function(){
        //console.log(this.value);
        $('#chapters .chapter').hide();
        //获取课程名称
        var id = '#' + this.value + '-wrapper';
        $(id).show();
    });
});
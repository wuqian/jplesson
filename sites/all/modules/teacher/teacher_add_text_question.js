$(document).ready(function(){
    $('#chapters .form-select').hide();
    $('#edit-course').change(function(){
        //console.log(this.value);
        $('#chapters .form-select').hide();
        //获取课程名称
        var id = '#'+this.value;
        $(id).show();
    });
});
$(document).ready(function(){
    $('#chapters .form-checkbox').hide();
    $('#edit-course').change(function(){
        //console.log(this.value);
        // $('#chapters .form-checkbox').hide();
        //获取课程名称
        var id = '#'+this.value;
        $(id).show();
    });
});
$(document).ready(function(){
    $('#chapters .form-select').hide();
    //console.log(this.value);
    //获取课程名称
    var id = '#'+$('#edit-course').val();

    $(id).show();
    
    $('#edit-course').change(function(){
        //console.log(this);
        $('#chapters .form-select').hide();
        //获取课程名称
        var id = '#'+this.value;
        $(id).show();
    });
});
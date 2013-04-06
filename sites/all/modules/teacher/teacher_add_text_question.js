$(document).ready(function(){
    $('#chapters .form-select').hide();

    //获取课程名称
    var id = '#'+$('#edit-course').val();
    $(id).show();
    var id = '#'+$('#edit-course').val() + '-wrapper';
    $(id).show();
    
    $('#edit-course').change(function(){
        $('#chapters .form-select').hide();
        //获取课程名称
        var id = '#'+this.value;
        $(id).show();
        var id = '#'+this.value + '-wrapper';
        $(id).show();
    });
});
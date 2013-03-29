<?php

function teacher_question_manage(){
    //试题管理目录
    $output = '';
    $output .= l("添加问答题","teacher/topic/add/text")."<br/>";
    $output .= l("添加选择题","teacher/topic/add/option")."<br/>";
    // $output .= l("编辑题目","teacher/topic/edit")."<br/>";
    // $output .= l("删除题目","teacher/topic/del")."<br/>";
    // $output .= l("导入Excel题库","")."<br/>";
    //显示所有试题
    $output .= drupal_get_form('topic_filter_form');
    $output .= drupal_get_form('topic_show_list_form');
    //var_dump($x);
    return $output;
}

function topic_show_list_form(){
    drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
    $form = array(
        '#attributes' => array('class' => 'teacher-all-options')
    );
    $topic_type = $_SESSION['topic_type'];
    $topic_class = $_SESSION['topic_class'];
    //krumo::dump($topic_type);
    //krumo::dump($topic_class);
    //topic_type:选择题/问答题
    //topic_class:课程
    if($topic_type == '所有'){
        $result = pager_query("SELECT * FROM {question} where question.course = '$topic_class'", 20, 0, NULL);
    }else {
        $result = pager_query("SELECT * FROM {question} where question.course = '$topic_class' and question.type = '$topic_type'", 20, 0, NULL);
    }
    while($term = db_fetch_object($result)){
        $results[] = $term;
    }

    if(!empty($results)) {
        foreach($results as $res){
            $form[$res->id]['#config'] = (array)$res;   
            $form[$res->id]['id'] = array('#type' => 'checkbox','#title' => check_plain($res->id));
            $form[$res->id]['topic'] = array('#value'=>check_plain($res->topic));
            $form[$res->id]['type'] = array('#value'=>check_plain($res->course));
            $form[$res->id]['edit'] = array('#value'=>l("编辑",'teacher/topic/edit/'.$res->id));
            $form[$res->id]['del'] = array('#value'=>l("删除",'teacher/topic/remove/'.$res->id));
        }
    }
    return $form;
}

function topic_filters(){
    $filters = array();
    $types = teacher_students_type1();

    $filters['topic_type'] = array(
        'options' => array(
        "所有"=>"所有",
        "option"=>"选择题",
        "text"=>"问答题")
    );

    $filters['topic_class'] = array(
        'title' => t('Type'),
        'where' => "u.type = '%s'",
        'join' => '',
        'options' => $types,
    );

    return $filters;
}

function topic_filter_form(){
    $session = &$_SESSION['teacher_choice_filter'];
    $session = is_array($session) ? $session : array();
    $filters = topic_filters();
    $form['filters'] = array(
        '#type' => 'fieldset',
        '#title' => t('显示指定试题类型和分类'),
        //'#theme' => 'teacher_students_filter',
    );

    foreach ($filters as $key => $filter) {
        $form['filters'][$key] = array(
            '#type' => 'select',
            '#options' => $filter['options'],
            '#default_value' => $_SESSION[$key],
        );
    }

    $form['filters']['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Show'),
        '#submit' => array('topic_filter_form_submit')
    );

    drupal_add_js('misc/form.js', 'core');

    return $form;
}

function teacher_text_question_form(&$form_state,$question_id){
    drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');
    
    if ($question_id) {
        $form['question_id'] = $question_id;
    }

    $form = array(
        '#attributes' => array('class' => 'teacher-form'),
    );

    //输入问答题-试题
    $form['topic'] = array(
        '#title' => 'Topic',
        '#type' => 'textarea',
        '#required' => true,
        '#default' => '',
    );

    //课程选项
    $filter = teacher_students_type_filters();
    $courses = $filter['type']['options'];
    $form['course'] = array(
        '#title' => '课程',
        '#type' => 'select',
        '#options' => $courses,
    );

    //查找每个课程的章节列表
    $form['chapter'] = array(
        '#title' => '章节',
        '#type' => 'fieldset',
        '#attributes' => array('id' => 'chapters'),
    );
    foreach ($courses as $key => $course) {
        $chapters = get_chapters($course);
        $form['chapter'][$course] = array(
            '#id'=>$course,
            '#type' => 'select',
            '#attributes' => array('style' => 'display:none;'),
            '#options' => $chapters,
        );
    }

    //score
    $form['score'] = array(
        '#title' => '分值',
        '#type' => 'textfield',
    );

    //level
    $form['level'] = array(
        '#title' => '难度等级',
        '#type' => 'select',
        '#options' => array(1,2,3,4),
    );

    //表单验证函数
    $form['#validate'] = array('add_text_question_validate');

        
    if ($form['question_id']){
        //已有question调用update接口
        
    }else{
        //新增question调用add接口
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'submit',
            '#submit' => array('add_text_question_submit'),
        );
    }


    return $form; 
}

//根据课程查找章节
function get_chapters($course){
    $types = array();
    $result = db_query('SELECT chapters from {node} where title = \'%s\'',$course);
    //krumo::dump($course);
    $chapters = array();
    while($result = db_fetch_object($result)){
        $chapters = unserialize(base64_decode($result->chapters));
    }
    return $chapters;
}

function update_text_question_submit(&$form,&$form_state){
    $course = $_POST['course'];
    $chapter = $_POST[$course];
    $score =  $_POST['score'];
    $level = $_POST['level'];
    $id = $_POST['question_id'];
    //UPDATE 表名称 SET 列名称 = 新值 WHERE 列名称 = 某值
    //Ex: UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
    db_query("UPDATE {question} SET topic = '%s', option_a = '%s', correct = '%d', 
            course = '%s', type = '%s', chapter = '%s', score = %d, level = %d where id = %d",
            $form_state['values']['topic'], 
            '', 
            '', 
            $course,
            'text',
            $chapter,
            intval($score),
            intval($level),
            intval($id)
            );

    if(db_last_insert_id('question', 'id')){
        drupal_set_message('Successful input');
    }else {
        drupal_set_message('Input failure','error');
    }
}
function add_text_question_submit(&$form,&$form_state){
    $course = $_POST['course'];
    $chapter = $_POST[$course];
    $score =  $_POST['score'];
    $level = $_POST['level'];

    db_query("INSERT INTO {question} (id,topic,option_a,correct,course,type,chapter,score,level)
            VALUES(null,'%s','%s','%d','%s','%s','%s','%d','%d')",
            $form_state['values']['topic'],'','',$course,'text',$chapter,intval($score),intval($level));

    if(db_last_insert_id('question', 'id')){
        drupal_set_message('Successful input');
    }else {
        drupal_set_message('Input failure','error');
    }
}

//试题删除
function topic_remove($id) {
  db_query('delete from {question} where id = %d',$id);
  drupal_goto("teacher/topic/manage");
}

//试题编辑
function topic_edit($id) {
    krumo::dump($id);

    //从数据库获取 question 对象
    $result = db_query('SELECT * FROM {question} where id = %d',intval($score));

    krumo::dump($result);
    //根据 question 对象类型 (问答题:text/选择题:option),进行不同的页面跳转
    if ($result->type == "text") {
        //跳转至问答题页面
        //todo:将result的结果传递给 teacher_text_question_form 
        $output = drupal_get_form('teacher_text_question_form');
        
    } elseif ($result->type == "option") {
        //跳转至选择题页面
        //todo:
    }

    return $output;
}

/**
 * 选择题试题录入
 */
function teacher_add_option_question(&$form_state){
  drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
  drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');
  
  $form = array(
    '#attributes' => array('class' => 'teacher-form'),
    );

  //课程选项
  $filter = teacher_students_type_filters();
  $courses = $filter['type']['options'];
  $form['course'] = array(
      '#title' => '课程',
      '#type' => 'select',
      '#options' => $courses,
      );

  //查找每个课程的章节列表
  $form['chapter'] = array(
    '#title' => '章节',
    '#type' => 'fieldset',
    '#attributes' => array('id' => 'chapters'),
    );
  foreach ($courses as $key => $course) {
    $chapters = get_chapters($course);
    $form['chapter'][$course] = array(
        '#id'=>$course,
        '#type' => 'select',
        '#attributes' => array('style' => 'display:none;'),
        '#options' => $chapters,
        );
}

  //score
$form['score'] = array(
    '#title' => '分值',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );

  //level
$form['level'] = array(
    '#title' => '难度等级',
    '#type' => 'select',
    '#options' => array(1,2,3,4),
    '#required' => true,
    '#default' => '',
    );

$form['topic'] = array(
    '#title' => '题目',
    '#type' => 'textarea',
    '#required' => true,
    '#default' => '',
    );

$options = array('A','B','C','D');
$form['radios'] = array(
  '#type' => 'radios',
  '#options' => $options,
  );

$form['option_A'] = array(
    '#title' => '选择 A',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );
$form['option_B'] = array(
    '#title' => '选择 B',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );
$form['option_C'] = array(
    '#title' => '选择 C',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );
$form['option_D'] = array(
    '#title' => '选择 D',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );


$form['#validate'] = array('add_option_question_validate');
$form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'submit',
    '#submit' => array('add_option_question_submit'),
    );
return $form;
}

function add_option_question_validate(&$form,&$form_state){
    if($form_state['values']['topic'] == ''){
        form_set_error('topic','The topic can not be empty');
    }
    else if($form_state['values']['option_A'] == ''){
        form_set_error('topic','The option can not be empty');
    }
    else if($form_state['values']['option_B'] == ''){
        form_set_error('topic','The option can not be empty');
    }
    else if($form_state['values']['option_C'] == ''){
        form_set_error('topic','The option can not be empty');
    }
    else if($form_state['values']['option_D'] == ''){
        form_set_error('topic','The option can not be empty');
    }
}

function add_option_question_submit(&$form,&$form_state){
  $a = $form_state['values']['option_A'];
  $b = $form_state['values']['option_B'];
  $c = $form_state['values']['option_C'];
  $d = $form_state['values']['option_D'];
  $teachersubject = array($a,$b,$c,$d);
  $con = serialize($teachersubject);
  $type = $_POST['type'];
  db_query("INSERT INTO {question} (id,topic,option_a,correct,questype,type)
   VALUES(null,'%s','%s','%d','%s','%s')",$form_state['values']['topic'],$con,$_POST[radios],$type,'option');
  if(db_last_insert_id('question', 'id')){
    drupal_set_message('Successful input');
}
else {
    drupal_set_message('Input failure','error');
}
}

function teacher_students_type() {
  $types = array();
  $result = db_query("SELECT DISTINCT title from {node} where type = 'lesson'");
  while($type = db_fetch_object($result)){
      $types['未分类'] = '未分类';
      $types[$type->title] = $type->title;
      
  }
  return array_filter($types);
}
function teacher_students_type_filters(){
  $filters = array();
  $types = teacher_students_type();
  $filters['type'] = array(
    'title' => t('Type'),
    'where' => "u.type = '%s'",
    'join' => '',
    'options' => $types,
    );
  return $filters;
}
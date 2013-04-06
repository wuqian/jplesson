<?php

function teacher_question_manage(){
    $output = '';
    $output .= l("添加问答题","teacher/question/add/text")."<br/>";
    $output .= l("添加选择题","teacher/question/add/option")."<br/>";

    $output .= drupal_get_form('teacher_question_filter_form');
    //$output .= drupal_get_form('teacher_question_list');
    //老代码
    //试题管理目录
 
    // $output .= l("编辑题目","teacher/topic/edit")."<br/>";
    // $output .= l("删除题目","teacher/topic/del")."<br/>";
    // $output .= l("导入Excel题库","")."<br/>";
    
    //显示所有试题
    //$output .= drupal_get_form('teacher_question_filter_form');
    $output .= drupal_get_form('teacher_question_list');
    //var_dump($x);
    return $output;
}

function topic_filter_form_submit(&$form,&$form_state){
  // var_dump($form_state);
  $_SESSION['topic_type'] = $form_state['values']['topic_type'];
  $_SESSION['topic_class'] = $form_state['values']['topic_class'];
}

function theme_teacher_question_list($form){
    //生成表头结构
    $header = array(
        array(),
        array('data' => t('试题编号'), 'field' => 'id', 'sort' => 'asc'),
        // array('data' => t('Fullname'), 'field' => 'u.fullname'),
        // array('data' => t('Class'), 'field' => 'u.class'),
        // array('data' => t('思科网院ID'), 'field' => 'c.acid'),
        // array('data' => t('思科网院用户名'), 'field' => 'c.username'),
        // array('data' => t('Status'), 'field' => 'u.status'),
        t('Operations')
    );
    $rows = array();
    // $output = drupal_render($form['options']);
    // if (isset($form['name']) && is_array($form['name'])) {
        foreach (element_children($form['id']) as $key) {
            krumo::dump($key);
            $rows[] = array(
                drupal_render($form['questions'][$key]),
            );
        }
    // }　else {
    //     $rows[] = array(array('data' => t('No users available.'), 'colspan' => '7'));
    // }

    $output .= theme('table', $header, $rows);
    if ($form['pager']['#value']) {
        $output .= drupal_render($form['pager']);
    }

    $output .= drupal_render($form);
}

//试题管理页面-试题列表
function teacher_question_list() {

    //设置表头:
    //试题编号:试题内容:试题类型:试题所属课程:试题所属章节:试题分值:试题难度等级
    // $header = array(
    //     array(),
    //     array('data' => t('content'), 'field' => 'topic', 'sort' => 'asc'),
    //     //t('Operations')
    // );

    // krumo::dump($header);
    // krumo::dump("test");
    $sql = "SELECT * FROM question";
    //$sql .= tablesort_sql($header);
    //krumo::dump($sql);
    $result = pager_query($sql, 50, 0, 10, NULL);

    // $form['options'] = array(
    //     '#type' => 'fieldset',
    //     '#title' => t('Update options'),
    //     '#prefix' => '<div class="container-inline">',
    //     '#suffix' => '</div>',
    // );
    // $options = array();
    // foreach (module_invoke_all('user_operations') as $operation => $array) {
    //     $options[$operation] = $array['label'];
    // }
    // $form['options']['operation'] = array(
    //     '#type' => 'select',
    //     '#options' => $options,
    //     '#default_value' => 'unblock',
    // );
    // $form['options']['submit'] = array(
    //     '#type' => 'submit',
    //     '#value' => t('Update'),
    // );
    //$destination = drupal_get_destination();

    //$status = array(t('blocked'), t('active'));
    //$roles = user_roles(TRUE);
    $questions = array();
    while ($question = db_fetch_object($result)) {
        krumo::dump($question->topic);

        $questions[$question->id] = '';
        $form['id'][$question->id] = array('#value' => $question->topic);

        // $form['fullname'][$account->uid] = array('#value' => $account->fullname);
        // $form['class'][$account->uid] = array('#value' => $account->class);
        // $form['acid'][$account->uid] = array('#value' => $account->acid);
        // $form['cisco_username'][$account->uid] = array('#value' => $account->username);
        // $form['status'][$account->uid] =  array('#value' => $status[$account->status]);
        // //$form['last_access'][$account->uid] =  array('#value' => $account->access ? t('@time ago', array('@time' => format_interval(time() - $account->access))) : t('never'));
        // $form['operations'][$account->uid] = array('#value' => l(t('edit'), "user/$account->uid/edit", array('query' => $destination)));
    }
    $form['questions'] = array(
        '#type' => 'checkboxes',
        '#options' => $questions
    );
    $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

    return $form;
}

function teacher_question_filter_form() {
    //获取filter
    $filters = topic_filters();

    krumo::dump($filters);

    $form['filters'] = array(
        '#type' => 'fieldset',
        //'#title' => t('Show only users where'),
        //'#theme' => 'teacher_students_filter',
    );

    foreach ($filters as $key => $filter) {
        $names[$key] = $filter['title'];
        $form['filters']['status'][$key] = array(
            '#type' => 'select',
            '#options' => $filter['options'],
        );
    }

    $form['filters']['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Show'),
    );

    return $form;
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
    //topic_type:  选择题/问答题
    //topic_class: 课程
    if($topic_type == '所有'){
        if ($topic_class == '所有') {
            //所有科目的所有题目
            $result = pager_query("SELECT * FROM {question} ", 20, 0, NULL);
        } else {
            //特定科目的所有题目
            $result = pager_query("SELECT * FROM {question} where question.course = '$topic_class'", 20, 0, NULL);
        }
        
    }else {
        if ($topic_class == '所有') {
            //所有科目的特定题目
            $result = pager_query("SELECT * FROM {question} where question.type = '$topic_type'", 20, 0, NULL);
        } else {
            //特定科目的特定题目
            $result = pager_query("SELECT * FROM {question} where question.course = '$topic_class' and question.type = '$topic_type'", 20, 0, NULL);
        }   
        
    }
    while($term = db_fetch_object($result)){
        $results[] = $term;
    }

    if(!empty($results)) {
        foreach($results as $res){
            $form[$res->id]['#config'] = (array)$res;   
            
            $form[$res->id]['id'] = array('#type' => 'checkbox','#title' => check_plain($res->id));

            $form[$res->id]['topic'] = array('#value'=>check_plain($res->topic));
            
            $form[$res->id]['course'] = array('#value'=>check_plain($res->course));

            if ($res->type == 'text') {
                $form[$res->id]['type'] = array('#value'=>check_plain('问答题'));
            } elseif ($res->type == 'option') {
                $form[$res->id]['type'] = array('#value'=>check_plain('选择题'));
            }
            
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



function teacher_text_question_form(&$form_state,$question){
    drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');

    krumo::dump($question);

    $form = array(
        '#attributes' => array(
            '#title' => 'Topic',
            'type' => 'textfield',
            ),
    );    

    $form['id'] = array(
        '#type' => 'value',
        '#value' => $question->id
    );

    //输入问答题-试题
    $form['topic'] = array(
        '#title' => 'Topic',
        '#type' => 'textarea',
        '#required' => true,
        '#default_value' => $question->topic,
    );

    //krumo::dump($question->course);
    //课程选项
    $filter = teacher_students_type_filters();
    $courses = $filter['type']['options'];
    $form['course'] = array(
        '#title' => '课程',
        '#type' => 'select',
        '#options' => $courses,
        '#default_value' => $question->course,
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
            '#default_value' => $question->chapter,
        );
    }

    //score
    $form['score'] = array(
        '#title' => '分值',
        '#type' => 'textfield',
        '#value' => $question->score
    );

    //level
    $form['level'] = array(
        '#title' => '难度等级',
        '#type' => 'select',
        '#options' => array(1,2,3,4),
        '#default_value' => $question->level,
    );

    //表单验证函数
    $form['#validate'] = array('add_text_question_validate');

        
    if ($question){
        //已有question调用update接口
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'submit',
            '#submit' => array('update_text_question_submit'),
        );
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
    $id = $form_state['values']['id'];

    //UPDATE 表名称 SET 列名称 = 新值 WHERE 列名称 = 某值
    //Ex: UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
    krumo::dump($id);
    db_query("UPDATE question SET topic = '%s', option_a = '%s', correct = '%d', 
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
function question_remove($id) {
  db_query('delete from {question} where id = %d',$id);
  drupal_goto("teacher/question/manage");
}

//试题编辑
function question_edit($id) {
    krumo::dump($id);
    //从数据库获取 question 对象
    $result = db_query('SELECT * FROM question WHERE id = %d', intval($id));

    $question;
    while ($q= db_fetch_object($result)) {
        $question = $q;
    }

    //根据 question 对象类型 (问答题:text/选择题:option),进行不同的页面跳转
    if ($result->type == "text") {
        //跳转至问答题页面
        //todo:将result的结果传递给 teacher_text_question_form 
        krumo::dump($question);
        $output = drupal_get_form('teacher_text_question_form',$question);
        
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
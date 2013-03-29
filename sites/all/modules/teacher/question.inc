<?php

function teacher_question_manage(){
  //试题管理目录
  $output = '';
  $output .= l("添加问答题","teacher/topic/add/text")."<br/>";
  $output .= l("添加选择题","teacher/topic/add/option")."<br/>";
  $output .= l("编辑题目","")."<br/>";
  $output .= l("删除题目","")."<br/>";
  $output .= l("导入Excel题库","")."<br/>";
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

  if($topic_class == '所有'){
    $result = pager_query("SELECT * FROM {question} where question.type = '$topic_type'", 20, 0, NULL);
  }else {
    $result = pager_query("SELECT * FROM {question} where question.questype = '$topic_class' and question.type = '$topic_type'", 20, 0, NULL);
  }
  while($term = db_fetch_object($result)){
    $results[] = $term;
  }
  if(!empty($results)) {
    foreach($results as $res){
      $form[$res->id]['#config'] = (array)$res;   
      $form[$res->id]['id'] = array('#type' => 'checkbox','#title' => check_plain($res->id));
      $form[$res->id]['topic'] = array('#value'=>check_plain($res->topic));
      $form[$res->id]['type'] = array('#value'=>check_plain($res->questype));
      $form[$res->id]['del'] = array('#value'=>l("删除",'teacher/topic/remove/'.$res->id));
    }
  }
  return $form;
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
    // $names[$key] = $filter['title'];
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

function teacher_add_text_question(&$form_state){
  drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');
  
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

  $form['#validate'] = array('add_text_question_validate');
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'submit',
    '#submit' => array('add_text_question_submit'),
  );

  return $form; 
}

//根据课程查找章节
function get_chapters($course){
  //todo:
  $types = array();
  $result = db_query('SELECT chapters from {node} where title = \'%s\'',$course);
 //krumo::dump($course);
  $chapters = array();
  while($result = db_fetch_object($result)){
    $chapters = unserialize(base64_decode($result->chapters));
  }
  return $chapters;
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
  }
  else {
    drupal_set_message('Input failure','error');
  }
}

function topic_remove($id) {
  db_query('delete from {question} where id = %d',$id);
  drupal_goto("teacher/topic/manage");
}

function teacher_add_option_question(&$form_state){
  drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
  $form = array(
    '#attributes' => array('class' => 'teacher-form'),
  );
  $filters = teacher_students_type_filters();
  $form['topic'] = array(
    '#title' => 'Topic',
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
    '#title' => 'optins A',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
  );
  $form['option_B'] = array(
    '#title' => 'Option B',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
  );
  $form['option_C'] = array(
    '#title' => 'Option C',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
  );
  $form['option_D'] = array(
    '#title' => 'Option D',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
  );
  foreach($filters as $key=>$filter){
    $form['type'] = array(
      '#title' => 'Type',
      '#type' => 'select',
      '#options' => $filter['options'],
    );
  }
 
  $form['#validate'] = array('add_option_question_validate');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'submit',
    '#submit' => array('add_option_question_submit'),
  );
  return $form;
}
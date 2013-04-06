<?php

/**
* 自动生成试卷表单
* @param  string $value [description]
* @return [type]        [description]
*/
function teacher_add_auto_questionpaper(&$form_state)
{
  drupal_add_js('sites/all/modules/teacher/teacher_add_auto_questionpaper.js','file');

  $form = array(
    '#attributes' => array('id' => 'add-auto-questionpaper-form'),
    );

//输入试卷标题
  $form['topic'] = array(
    '#title' => '标题',
    '#type' => 'textfield',
    '#required' => true,
    '#default' => '',
    );

  $form['class_id'] = array(
    '#title' => '班级',
    '#type' => 'select',
    '#options' => getClassesOptionList(),
    );

//课程选项
  $courses = getCoursesOptionList();

  $keys = array_keys($courses);
  $courseSelected = $courses[$keys[1]];

  $form['course'] = array(
    '#title' => '课程',
    '#type' => 'select',
    '#options' => $courses,
    '#default_value' => $courseSelected,
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
      '#type' => 'item',
      '#attributes' => array('class' => 'chapter'),
      );
    $form['chapter'][$course][$course] = array(
      '#type' => 'checkboxes',
      '#options' => $chapters
      );
  }

//score
  $form['score'] = array(
    '#title' => '总分值',
    '#type' => 'textfield',
    );

//level
  $form['level'] = array(
    '#title' => '难度等级',
    '#type' => 'select',
    '#options' => array('A','B','C','D'),
    );

  $form['#validate'] = array('teacher_add_auto_questionpaper_validate');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => '生成',
    '#submit' => array('teacher_add_auto_questionpaper_submit'),
    );

  return $form; 
}

function teacher_add_auto_questionpaper_submit(&$form,&$form_state){
  krumo::dump($form_state);
  // $a = $form_state['values']['option_A'];
  // $b = $form_state['values']['option_B'];
  // $c = $form_state['values']['option_C'];
  // $d = $form_state['values']['option_D'];
  // $teachersubject = array($a,$b,$c,$d);
  // $con = serialize($teachersubject);
  // $type = $_POST['type'];
  // db_query("INSERT INTO {question} (id,topic,option_a,correct,questype,type)
  //          VALUES(null,'%s','%s','%d','%s','%s')",$form_state['values']['topic'],$con,$_POST[radios],$type,'option');
  // if(db_last_insert_id('question', 'id')){
  //   drupal_set_message('Successful input');
  // }
  // else {
  //   drupal_set_message('Input failure','error');
  // }
}

function teacher_add_auto_questionpaper_validate(&$form,&$form_state) {

}


/**
 * 获取班级列表
 */
function getClassesOptionList()
{
  $classes = array();
  $result = db_query('SELECT DISTINCT u.class FROM {users} u LEFT JOIN {users_roles} ur ON u.uid = ur.uid WHERE ur.rid = 4');
  while ($class = db_fetch_object($result)) {
    $classes[$class->class] = $class->class;
  }
  return array_filter($classes);
}

/**
 * 获取所有课程选项列表
 */
function getCoursesOptionList()
{
  $options = array();
  $result = db_query("SELECT DISTINCT title from {node} where type = 'lesson'");
  while($option = db_fetch_object($result)){
    $options['未分类'] = '未分类';
    $options[$option->title] = $option->title;

  }
  return $options;
}

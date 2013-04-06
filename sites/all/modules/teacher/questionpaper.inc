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
  $form['title'] = array(
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
    '#required' => true,
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

  $paperParams = array();
  $paperParams['course'] = $form_state['values']['course'];
  $paperParams['chapters'] = $form_state['values'][$paperParams['course']];
  $paperParams['totalScore'] = $form_state['values']['score'];

  $paper = generateQuestionPaper($paperParams);

  if ($paper == FALSE) {
    drupal_set_message('试题数量不足','error');
    return;
  }
  //试卷插入数据库
  $paper = serialize($paper);
  db_query("INSERT INTO {questionpaper} (title,class_id,questions)
            VALUES('%s','%s','%s')", $form_state['values']['title'], $form_state['values']['class_id'], $paper);

  if(db_last_insert_id('questionpaper', 'pid')){
    drupal_set_message('试卷生成成功!');
  }
  else {
    drupal_set_message('试卷生成失败, 请重试。','error');
  }
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


/**
 * 自动生成试卷
 * @param  string $params [description]
 * @return array  $paper  [description]
 *         FALSE  试题数量不足               
 */
function generateQuestionPaper($params)
{
  $chapterQuery = array();
  foreach ($params['chapters'] as $chapter => $selected) {
    if ($selected) {
      $chapterQuery[] = $chapter;
    }
  }
  $chapterQuery = implode(",", $chapterQuery);

  $result = db_query('SELECT * FROM {question} WHERE course = "%s" AND chapter IN (%s)', $params['course'], $chapterQuery);

  //取得指定课程章节所有试题
  $questions = array();
  while($question = db_fetch_object($result)) {
    $questions[] = $question;
  }
  //打乱顺序
  shuffle($questions);

  $paper = array();
  $totalScore = $params['totalScore'];
  $currentScore = 0;

  while($currentScore < $totalScore) {
    if(!count($questions)) break;
    $questionPop = array_pop($questions);
    $paper[] = $questionPop->id;
    $currentScore += $questionPop->score;
  }

  if ($currentScore < $totalScore) {
    return FALSE; //试题数量不足总分
  }

  return $paper;
}

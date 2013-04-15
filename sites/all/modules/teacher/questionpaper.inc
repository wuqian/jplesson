<?php

/**
* 自动生成试卷表单
* @param  string $value [description]
* @return [type]        [description]
*/
function teacher_add_auto_questionpaper()
{
  if (!empty($_POST)) {
    teacher_add_auto_questionpaper_submit($_POST);
  }

  drupal_add_js('sites/all/modules/teacher/teacher_add_auto_questionpaper.js','file');

  $output = '';

  $classes = getClassesOptionList();
  $courses = getCoursesOptionList();

  $params = array();

  $params['classes'] = $classes;

  foreach ($courses as $key => $course) {
    $chapters = get_chapters($course);
    $params['courses'][] = array(
      'name' => $course,
      'chapters' => $chapters,
      );
  }

  $output = theme('add_auto_questionpaper', $params);
  
  return $output;

}

function teacher_add_auto_questionpaper_submit($post){

  //检验表单提交项
  if (empty($post['title'])) {
    drupal_set_message('请填写试卷标题','error');
    return;
  }
  
  if (empty($post['class_ids'])) {
    drupal_set_message('请选择试卷目标班级','error');
    return;
  }
    
  //检验总题数大于0
  $totleQuestionCount = 0;
  for ($i=0; $i < $post['chapterCount']; $i++) { 
    $totleQuestionCount += array_sum($post['chapter-' . $i]);
  }
  if ($totleQuestionCount == 0) {
    drupal_set_message('请设置试题数量','error');
    return;
  }


  $paperParams = array();
  $paperParams['course'] = $post['course'];
  $paperParams['chapterCount'] = $post['chapterCount'];
  
  $paperParams['chapters'] = array();

  //获取post中的章节题型难度分布数据
  for ($i=0; $i < $post['chapterCount']; $i++) { 

    $paperParams['chapters'][$i] = $post['chapter-' . $i];

    //检测类型与难度数量设置匹配
    $typeCount = $post['chapter-' . $i]['option'] +
                 $post['chapter-' . $i]['multiple_option'] +
                 $post['chapter-' . $i]['true_false'] +
                 $post['chapter-' . $i]['text']; 
    $levelCount = $post['chapter-' . $i]['level1'] +
                  $post['chapter-' . $i]['level2'] +
                  $post['chapter-' . $i]['level3'] +
                  $post['chapter-' . $i]['level4']; 
    if ($typeCount != $levelCount) {
      drupal_set_message('类型与难度题目数量设置不匹配','error');
      return;
    }

  }

  
  $paper = generateQuestionPaper($paperParams);

  if ($paper == FALSE) {
    drupal_set_message('试题数量不足','error');
    return;
  }

  //试卷插入数据库
  $paper = serialize($paper);

  $classId = serialize($post['class_ids']);

  db_query("INSERT INTO {questionpaper} (title,class_id,questions)
            VALUES('%s','%s','%s')", $post['title'], $classId, $paper);

  if(db_last_insert_id('questionpaper', 'pid')){
    drupal_set_message('试卷生成成功!');
    drupal_goto('teacher/questionpaper/manage');
  }
  else {
    drupal_set_message('试卷生成失败, 请重试。','error');
  }
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
  $paper = array();

  foreach ($params['chapters'] as $key => $chapter) {

    //通过章节id查询章节名称
    $chapters = get_chapters($params['course']);
    $chapterQuery = $chapters[$key];
    $result = db_query('SELECT * FROM {question} WHERE course = "%s" AND chapter = "%s"', $params['course'], $chapterQuery);
    
    while($question = db_fetch_object($result)) {
      $questions[] = $question;
    }
    shuffle($questions);

    //用试题填充条件
    foreach ($questions as $key => $question) {
      if (testInsert($chapter, $question)) {
        $paper[] = $question->id;
        $chapter[$question->type] -= 1;
        $chapter['level' . $question->level] -= 1;
      }

      // krumo::dump(isChapterFull($chapter));
      //装满退出
      if (isChapterFull($chapter)) {
        break;
      }
    }

    if (!isChapterFull($chapter)) {
      return FALSE;//试题数量不足
    }

  }
  return $paper;
}

function isChapterFull($chapter) {
  $sum = $chapter['option'] + $chapter['multiple_option'] + $chapter['true_false'] + $chapter['text'] + 
  $chapter['level1'] + $chapter['level2'] + $chapter['level3'] + $chapter['level4'];
  if ($sum == 0) {
    return TRUE;
  }
  return FALSE;
}

function testInsert($chapter, $question) {
  $typeReq = $chapter[$question->type] - 1;
  $levelReq = $chapter['level' . $question->level] - 1;

  if ($typeReq >= 0 && $levelReq >= 0) {
    return TRUE;
  }
  return FALSE;
}

function teacher_add_manually_questionpaper()
{
    drupal_add_css(drupal_get_path('module', 'teacher') .'/add_manually_questionpaper.css', 'module', 'all', FALSE);
    $output = drupal_get_form('teacher_choice_filter_form');
    $output .= drupal_get_form('teacher_all_options');
    return $output;
}

function teacher_all_options(&$form_state){
    drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
    $form = array();
    $form = array(
        '#attributes' => array('class' => 'teacher-all-options')
    );
  
    //试题科目
    $course = $_SESSION['teacher_choice_filter'][0];

    if($course == '所有'){
        $result = pager_query("SELECT * FROM {question}", 20, 0, NULL);
    }
    else {
        $result = pager_query("SELECT * FROM {question} where course = '".$course."'", 20, 0, NULL);
    }
    
    $filters = teacher_students_filters();  
    $filters1 = teacher_students_type_filters();
    $form = array('#tree' => TRUE);
    //过滤
    
    $form['options'] = array(
        '#type' => 'fieldset',
    );
    //krumo::dump($filters['class']);
    $form['options']['title'] = array(
            '#title' => '试卷名称',
            '#type' => 'textfield',
    ); 

    $form['options']['available_class_id'] = array();
    foreach ($filters['class']['options'] as $key => $class_id) {
        $form['options']['available_class_id'][$class_id] = array(
            '#title' => $class_id,
            '#type' => 'checkbox',
        );
    }

    while($term = db_fetch_object($result)){
        $results[] = $term;
    }

    if(!empty($results)) {
        foreach($results as $res){
            $form[$res->id]['#config'] = (array)$res;       
            $form[$res->id]['id'] = array('#type' => 'checkbox','#title' => check_plain($res->id));
            $form[$res->id]['topic'] = array('#value'=>check_plain($res->topic));
            $form[$res->id]['type'] = array('#value'=>check_plain($res->type));
            $form[$res->id]['score'] = array('#value'=>check_plain($res->score));
            $form[$res->id]['level'] = array('#value'=>check_plain($res->level));
        }
    }

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => '生成试卷',
        '#submit' => array('teacher_questionpaper_submit'),
    );
    
    if(count($res)>1){
        $form['submit'] = array('#type'=>'submit','#value' => t('save'));
    }

    $form['pager'] = array('#value' => theme('pager', null, 20, 0));
    return $form;
}

function theme_teacher_all_options($form){
    $header = array(t('编号'),t('试题列表'),t('试题类型'),t('分值'),t('难度'));

    $rows = array();
    $output = drupal_render($form['options']);
    foreach(element_children($form) as $key){
        if(isset($form[$key]['id'])){
            $vocabulary = &$form[$key];
            $row = array();
            $row[] = drupal_render($vocabulary['id']);
            $row[] = drupal_render($vocabulary['topic']);

            $type = drupal_render($vocabulary['type']);
            if ($type == 'text') {
                $row[] = t('问答题');
            } elseif ($type == 'option') {
                $row[] = t('选择题');
            } elseif ($type == 'true_false') {
                $row[] = t('判断题');
            } elseif ($type == 'multiple_option') {
                $row[] = t('多选题');
            } else {
                $row[] = t('type unknown');
            }

            $row[] = drupal_render($vocabulary['score']);
            $row[] = drupal_render($vocabulary['level']);


            if(isset($vocabulary['weight'])){
                $vocabulary['weight']['#attributes']['class'] = 'vocabulary-weight';
                $row[] = drupal_render($vocabulary['weight']);
            }
            $rows[] = array('data'=>$row,'class'=>'teacher-options-list');
        }
    }
    if(empty($rows)){
        $rows = array(array('data'=>t('No vocalable'),'colspan'=>2));
    }
    
    $output .= theme('table',$header,$rows,array('id'=>'teacher-options-list')).drupal_render($form);
    return $output; 
}

/*
 * 点击生成手工试卷
 */
function teacher_questionpaper_submit(&$form,&$form_state){

    $class_id_list = $form_state['values']['options']['available_class_id'];
    $selected_class = array();
    foreach ($class_id_list as $class_id => $select) {
      if ($select) {
        $selected_class [] = $class_id;
      }
    }

    //krumo::dump($selected_class);
    
    //todo:表单验证
    if (!$selected_class) {
        krumo::dump("班级未选择");
        return;
    }

    //将试卷所对应的班级进行序列化
    $selected_class = serialize($selected_class);

    $title = $form_state['values']['options']['title'];

    $h = db_result(db_query("select id from {question} limit 0,1"));
    $e = db_result(db_query("select id from {question} order by id desc limit 0,1"));

    //将题目id存入数组,序列化到questions字段中
    $questions = array();
    for($i=$h;$i<=$e;$i++){
        if($_POST[$i]){
            $questions [] = $i;
        }
    }

    db_query("insert into {questionpaper} (class_id,questions,title)
            values ('%s','%s','%s')",$selected_class,serialize($questions),$title);
}

/*
 * 试卷列表
 */

function teacher_questionpaper_manage()
{
    //$output = drupal_get_form('teacher_topic_filter_form');
    $output .= drupal_get_form('teacher_question_paper_list');
    return $output;
}

function teacher_question_paper_list(&$form_state){
    //实现一张试卷对应多个班级的情况
    //$class = $_SESSION['teacher_topic_filter'][0];

    $class = '所有';
    if($class == '所有'){
        $result = pager_query("SELECT * FROM {questionpaper}", 20, 0, NULL);
    }
    else {
        $result = pager_query("SELECT * FROM {questionpaper} where class_id = '$class'", 20, 0, NULL);
    }
    
    while($term = db_fetch_object($result)){
        $results[] = $term;
    }

    $form = array('#tree' => TRUE);
    if(!empty($results)) {
        foreach($results as $res){
            $form[$res->pid]['#config'] = (array)$res;
            $form[$res->pid]['pid'] = array('#value'=>check_plain($res->pid));
            $form[$res->pid]['title'] = array('#value'=>check_plain($res->title));
            //todo:对应班级加url
            $class_id = implode(',', unserialize($res->class_id));
            $form[$res->pid]['class_id'] = array('#value'=>check_plain($class_id));
            $form[$res->pid]['check'] = array('#value'=>l(t('查看'),"teacher/topic/paper/".check_plain($res->pid)));
        } 
    }

    if(count($res)>1){
        $form['submit'] = array('#type'=>'submit','#value' => t('save'));
        }
    else if(isset($res)){
        unset($form[$res->pid]['weight']);
    }
    $form['pager'] = array('#value' => theme('pager', null, 20, 0));
    return $form;
}

function theme_teacher_question_paper_list($form){
    $rows = array();
    foreach(element_children($form) as $key){
        if(isset($form[$key]['pid'])){
            $vocabulary = &$form[$key];
            
            $row = array();
            $row[] = drupal_render($vocabulary['pid']);
            $row[] = drupal_render($vocabulary['title']);
            $row[] = drupal_render($vocabulary['class_id']);
            $row[] = drupal_render($vocabulary['check']);
            if(isset($vocabulary['weight'])){
                $vocabulary['weight']['#attributes']['class'] = 'vocabulary-weight';
                $row[] = drupal_render($vocabulary['weight']);
            }
            $rows[] = array('data'=>$row,'class'=>'teacher-list');
        }
    }
    if(empty($rows)){
        $rows = array(array('data'=>t('没有找到试卷'),'colspan'=>2));
    }

    $header = array(t('试卷ID'),t('试卷名称'),t('班级'),t('编辑'));
    if(isset($form['submit'])){
        $header[] = t('Weight');
        drupal_add_tabledrag('taxonomy','order','sibling','vocabulary-weight');
    }

    return theme('table',$header,$rows,array('id'=>'teacher-list')).drupal_render($form);
}

/**
 * 试卷批改列表页
 */
function questionpaper_answer_list(&$form_state, $questionpaper_id){

  $result = pager_query("SELECT a.*, u.name FROM {answer} a LEFT JOIN {users} u ON a.student_id = u.uid WHERE questionpaper_id=$questionpaper_id", 50, 0, NULL);
  while($term = db_fetch_object($result)){
    $results[] = $term;
  }

  foreach($results as $res){
    $form['#items'][$res->id]['student_id'] = array('#value'=>check_plain($res->student_id));
    $form['#items'][$res->id]['name'] = array('#value'=>check_plain($res->name));
    $form['#items'][$res->id]['stat'] = array('#value'=>check_plain($res->stat));
    $form['#items'][$res->id]['score'] = array('#value'=>check_plain($res->score));
    $op;
    switch ($res->stat) {
      case 0:
      $op = array('#value' => t('等待自动批改'));
        break;
      case 1:
      $op = array('#value' => l(t('批改'), "teacher/questionpaper/". $res->id ."/correct"));
        break;
      case 2:
      $op = array('#value' => t('已批改'));
        break;
      default:
        break;
    }
    $form['#items'][$res->id]['operation'] = $op;
  }

  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
  
  return $form;
}

function questionpaper_answer_list_submit(){

}

function theme_questionpaper_answer_list($form){

  $rows = array();

  foreach(element_children($form['#items']) as $key){
      $item = &$form['#items'][$key];
      $row = array();
      $row[] = drupal_render($item['student_id']);
      $row[] = drupal_render($item['name']);
      $row[] = drupal_render($item['stat']);
      $row[] = drupal_render($item['score']);
      $row[] = drupal_render($item['operation']);
      $rows[] = array('data' => $row);
  }

  if(empty($rows)){
    $rows = array(array('data' => t('没有提交记录'),'colspan'=>2));
  }

  $header = array(t('学号'),t('姓名'),t('批改状态'),t('得分'),t('操作'));


  return theme('table',$header,$rows).drupal_render($form);
  
}


/**
 * 试卷批改页
 */

function questionpaper_answer_correct(&$form_state, $questionpaper_id){
  $form = array();
    //获取问题列表
  $query = db_query("SELECT a.*, q.* FROM {answer} a LEFT JOIN {questionpaper} q ON a.questionpaper_id = q.pid WHERE a.id = %d", $questionpaper_id);
  $paperAnswered = db_fetch_object($query);  
  $questions = unserialize($paperAnswered->questions);

  //从数据库中获取question的详细信息
  $result = db_query('SELECT * FROM {question} WHERE id IN (%s) ORDER BY type ASC', implode(",", $questions));

  $answer = unserialize($paperAnswered->answers);
  $index = 1;
  while($question = db_fetch_object($result)){
        //为题目创建form:(区分选择题＆问答题)
    if ($question->type == 'text') {
            //问答题
      $form[][$question->id]['question'] = array(
        '#type' => 'item',
        '#title' => $index .'.'.$question->topic,
        );

      if (empty($answer[$question->id])) {
        $answer[$question->id] = '未填写答案';
      }
      $form[][$question->id]['answer'] = array(
        '#type' => 'item',
        '#value' => $answer[$question->id],
        );
      $form[][$question->id]['correct' . $question->id] = array(
        '#type' => 'textfield',
        '#field_prefix' => '得分: ',
        '#size' => 10, 
        '#required' => TRUE,
        );
    } else {
      //其他类型题目自动判卷
      //krumo::dump("error");
    }
    $index ++;

  }

  $form['baseScore'] = array(
    '#type' => 'value',
    '#value' => $paperAnswered->score,
    );

  $form['answerId'] = array(
    '#type' => 'value',
    '#value' => $paperAnswered->id,
    );

  $form['destination'] = array(
    '#type' => 'hidden',
    '#value' => 'teacher/topic/paper/'. $paperAnswered->pid . '/answer'
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('提交'),
    '#submit' => array('questionpaper_answer_correct_submit')
    );

  return $form;

}


function questionpaper_answer_correct_submit(&$form,&$form_state){

  $scoreAdd = 0;
  foreach ($form_state['values'] as $key => $value) {
    $pattern = '/^correct.*/';
    if(preg_match($pattern, $key)) {
      $scoreAdd += intval($value);
    }
  }

  $finalScore = $form_state['values']['baseScore'] + $scoreAdd;

  $result = db_query("UPDATE answer SET score = %d, stat = %d where id = %d AND stat = 1",
    intval($finalScore),
    2,
    intval($form_state['values']['answerId'])
    );

  if($result){
    drupal_set_message('批改成功');
    $form_state['redirect'] = $form_state['values']['destination'];
  }else {
    drupal_set_message('输入错误，请重试','error');
  }

}
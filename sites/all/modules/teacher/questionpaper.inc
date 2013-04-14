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

function teacher_add_manually_questionpaper()
{
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

    $form['options']['available_class_id'] = array(
        '#title' => '班级列表',
        '#type' => 'select',
        '#multiple' => true,
        '#enable'=>true,
        '#options' => $filters['class']['options'],
        //'#attributes'=>array('style' => 'width:100'),
    );

    $form['options']['add_class_id'] = array(
        '#value' => 'Add',
        '#type' => 'submit',
        '#submit' => array('teacher_questionpaper_add_class'),
    );

    $form['options']['remove_class_id'] = array(
        '#value' => 'Remove',
        '#type' => 'submit',
        '#submit' => array('teacher_questionpaper_remove_class'),
    );

    $form['options']['selected_class_id'] = array(
        '#title' => '已选班级列表',
        '#type' => 'select',
        '#multiple' => true,
        '#enable'=>true,
        '#size' => 5,
        //'#attributes'=>array('style' => 'width:100'),
        '#options' =>  $_SESSION['selected_class_id'],
    );

    $form['options']['title'] = array(
            '#title' => '试卷名称',
            '#type' => 'textfield',
    ); 

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

//将选中的classid,从session中删除
function teacher_questionpaper_remove_class(&$form,&$form_state){
    $selected_class_id = $form_state['values']['options']['selected_class_id'];
    $_SESSION['selected_class_id'] = array_diff($_SESSION['selected_class_id'],$selected_class_id);
}

//将选中的classid,加入到session中
function teacher_questionpaper_add_class(&$form,&$form_state){
    $selected_class_id = $form_state['values']['options']['available_class_id'];
    krumo::dump($class_id);
    if (!$_SESSION['selected_class_id']){
        $_SESSION['selected_class_id'] = $selected_class_id;
    } else {
        $_SESSION['selected_class_id'] = array_merge($_SESSION['selected_class_id'], $selected_class_id);
    }
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
            if (drupal_render($vocabulary['type']) == text) {
                $row[] = t('问答题');
            }elseif (drupal_render($vocabulary['type']) == option) {
                $row[] = t('选择题');
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
    //$class_id = $form_state['values']['options']['class_id'];
    $selected_class_id = $_SESSION['selected_class_id'];

    //删除session中的残留值
    $_SESSION['selected_class_id'] = NULL;
    
    //todo:表单验证
    if (!$selected_class_id) {

        return;
    }
    //将试卷所对应的班级进行序列化
    $selected_class_id = serialize($selected_class_id);

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
            values ('%s','%s','%s')",$selected_class_id,serialize($questions),$title);
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
    } elseif ($question->type == 'option') {

      //选择题不进行批改


      //       //选择题
      // $form[][$question->id] = array(
      //   '#type' => 'item',
      //           //'#default_value' => '1',
      //   '#title' => $index .'.'.$question->topic,
      //   );
      // $arr = unserialize($question->option_a);
      // $form[][$question->id] = array(
      //   '#type' => 'radios',
      //   '#options' => $arr,
      //   );
    } else {
      krumo::dump("error");
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
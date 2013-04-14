<?php

function teacher_question_manage(){
  $output = '';
  //$output .= l("添加问答题","teacher/question/add/text")."<br/>";
  //$output .= l("添加选择题","teacher/question/add/option")."<br/>";

  $output .= drupal_get_form('teacher_question_filter_form');
  $output .= drupal_get_form('teacher_question_list');
    //var_dump($x);
  return $output;
}

function question_filter_form_submit(&$form,&$form_state){
  $_SESSION['topic_type'] = $form_state['values']['topic_type'];
  $_SESSION['topic_class'] = $form_state['values']['topic_class'];
}

function theme_teacher_question_list($form){
    //生成表头结构
    //试题编号:试题内容:试题类型:试题所属课程:试题所属章节:试题分值:试题难度等级
  $header = array(
    array('data' => t('试题编号'), 'field' => 'id', 'sort' => 'asc'),
    array('data' => t('试题内容'), 'field' => 'topic', 'sort' => 'asc'),
    array('data' => t('试题类型'), 'field' => 'type', 'sort' => 'asc'),
    array('data' => t('试题所属课程'), 'field' => 'course', 'sort' => 'asc'),
    array('data' => t('试题所属章节'), 'field' => 'chapter', 'sort' => 'asc'),
    array('data' => t('试题分值'), 'field' => 'score', 'sort' => 'asc'),
    array('data' => t('试题难度等级'), 'field' => 'level', 'sort' => 'asc'),
    );

  $rows = array();
    // $output = drupal_render($form['options']);
  if (isset($form['id']) && is_array($form['id'])) {
    foreach (element_children($form['id']) as $key) {
            //krumo::dump($form['operations'][$key]);
      $rows[] = array(
        drupal_render($form['id'][$key]),
        drupal_render($form['topic'][$key]),
        drupal_render($form['type'][$key]),
        drupal_render($form['course'][$key]),
        drupal_render($form['chapter'][$key]),
        drupal_render($form['score'][$key]),
        drupal_render($form['level'][$key]),
        drupal_render($form['edit'][$key]),
        drupal_render($form['remove'][$key]),
        );
    }
  } else {
    $rows[] = array(array('data' => t('No users available.'), 'colspan' => '7'));
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;
}

//试题管理页面-试题列表
function teacher_question_list() {

    //设置表头:
    //试题编号:试题内容:试题类型:试题所属课程:试题所属章节:试题分值:试题难度等级
  $header = array(
    array('data' => t('试题编号'), 'field' => 'id', 'sort' => 'asc'),
    array('data' => t('试题内容'), 'field' => 'topic', 'sort' => 'asc'),
    array('data' => t('试题类型'), 'field' => 'type', 'sort' => 'asc'),
    array('data' => t('试题所属课程'), 'field' => 'course', 'sort' => 'asc'),
    array('data' => t('试题所属章节'), 'field' => 'chapter', 'sort' => 'asc'),
    array('data' => t('试题分值'), 'field' => 'score', 'sort' => 'asc'),
    array('data' => t('试题难度等级'), 'field' => 'level', 'sort' => 'asc'),
    );

  $topic_type = $_SESSION['topic_type'];
  $topic_class = $_SESSION['topic_class'];
    // krumo::dump($header);
  $sql = '';
  if($topic_type == '所有'){
    if ($topic_class == '所有') {
            //所有科目的所有题目
      $sql = "SELECT * FROM question";
    } else {
            //特定科目的所有题目
      $sql = "SELECT * FROM {question} where question.course = '$topic_class'";
    }    
  }else {
    if ($topic_class == '所有') {
            //所有科目的特定题目
      $sql = "SELECT * FROM {question} where question.type = '$topic_type'";
    } else {
            //特定科目的特定题目
      $sql = "SELECT * FROM {question} where question.course = '$topic_class' and question.type = '$topic_type'";
    }   
  }

  $sql .= tablesort_sql($header);
  $result = pager_query($sql, 50, 0, 10, NULL);

  $questions = array();
  while ($question = db_fetch_object($result)) {
    $questions[$question->id] = '';
    $form['id'][$question->id] = array('#value' => $question->id);
    $form['topic'][$question->id] = array('#value' => $question->topic);
    $form['type'][$question->id] = array('#value' => $question->type);
    $form['course'][$question->id] = array('#value' => $question->course);
    $form['chapter'][$question->id] = array('#value' => $question->chapter);
    $form['score'][$question->id] = array('#value' => $question->score);
    $form['level'][$question->id] = array('#value' => $question->level);
        //teacher/question/edit/%
    $form['edit'][$question->id] = array('#value' => l(t('edit'), 'teacher/question/edit/'.$question->id, array('query' => $destination)));
        //teacher/question/remove/%
    $form['remove'][$question->id] = array('#value' => l(t('delete'), 'teacher/question/remove/'.$question->id, array('query' => $destination)));
  }

  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

  return $form;
}

function teacher_question_filter_form() {
    //获取filter
  $filters = topic_filters();

  $topic_type = $_SESSION['topic_type'];
  $topic_class = $_SESSION['topic_class'];

    //krumo::dump($filters);

  $form['filters'] = array(
    '#type' => 'fieldset',
        //'#title' => t('Show only users where'),
        //'#theme' => 'teacher_students_filter',
    );

  $form['filters']['status']['topic_type'] = array(
    '#type' => 'select',
    '#options' => $filters['topic_type']['options'],
    '#default_value' => $topic_type
    );
  $form['filters']['status']['topic_class'] = array(
    '#type' => 'select',
    '#options' => $filters['topic_class']['options'],
    '#default_value' => $topic_class
    );

  $form['filters']['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Show'),
    '#submit' => array('question_filter_form_submit')
    );

  return $form;
}


function topic_filters(){
  $filters = array();
  $types = teacher_students_type1();

  $filters['topic_type'] = array(
    'options' => array(
      "所有"=>"所有",
      "option"=>"选择题",
      "text"=>"问答题",
      "true_false"=>"判断题",
      "multiple_option"=>"多选题")
    );

  $filters['topic_class'] = array(
    'title' => t('Type'),
    'where' => "u.type = '%s'",
    'join' => '',
    'options' => $types,
    );

  return $filters;
}
//
function teacher_text_question_form(&$form_state,$question){
  drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');

  krumo::dump($question);

  $form = array(
    '#attributes' => array(
      '#title' => 'Topic',
      'type' => 'textfield',
      ),
    );    

  $form['type'] = array(
    '#type' => 'value',
    '#value' => "text"
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
      '#submit' => array('update_question_submit'),
      );
  }else{
        //新增question调用add接口
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'submit',
      '#submit' => array('add_question_submit'),
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

//更新问题接口
function update_question_submit(&$form,&$form_state){
  $course = $_POST['course'];
  $chapter = $_POST[$course];
  $score =  $_POST['score'];
  $level = $_POST['level'];
  $id = $form_state['values']['id'];
  $type = $form_state['values']['type'];

  $a = $form_state['values']['option_A'];
  $b = $form_state['values']['option_B'];
  $c = $form_state['values']['option_C'];
  $d = $form_state['values']['option_D'];
  $teachersubject = array($a,$b,$c,$d);
  $con = serialize($teachersubject);

    //UPDATE 表名称 SET 列名称 = 新值 WHERE 列名称 = 某值
    //Ex: UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
  krumo::dump($id);
  db_query("UPDATE question SET topic = '%s', option_a = '%s', correct = '%d', 
    course = '%s', type = '%s', chapter = '%s', score = %d, level = %d where id = %d",
    $form_state['values']['topic'], 
    $con, 
    $_POST['radios'], 
    $course,
    $type,
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
    //drupal_goto("teacher/question/manage");
  $form_state['redirect'] = 'teacher/question/manage';
}

//新增问题接口
function add_question_submit(&$form,&$form_state){
  $course = $_POST['course'];
  $chapter = $_POST[$course];
  $score =  $_POST['score'];
  $level = $_POST['level'];

    //序列化数组
  $a = $form_state['values']['option_A'];
  $b = $form_state['values']['option_B'];
  $c = $form_state['values']['option_C'];
  $d = $form_state['values']['option_D'];
  $teachersubject = array($a,$b,$c,$d);
  $con = serialize($teachersubject);

    //问题类型:option/text
  $type = $form_state['values']['type'];

  db_query("INSERT INTO {question} (id,topic,option_a,correct,course,type,chapter,score,level)
    VALUES(null,'%s','%s','%d','%s','%s','%s','%d','%d')",
    $form_state['values']['topic'],$con,$_POST['radios'],$course,$type,$chapter,intval($score),intval($level));

  if(db_last_insert_id('question', 'id')){
    drupal_set_message('Successful input');
  }else {
    drupal_set_message('Input failure','error');
  }

  $form_state['redirect'] = 'teacher/question/manage';
}

//试题删除接口
function question_remove($id) {
  db_query('delete from {question} where id = %d',$id);
  drupal_goto("teacher/question/manage");
}

//试题编辑接口
function question_edit($id) {
  krumo::dump($id);
    //从数据库获取 question 对象
  $result = db_query('SELECT * FROM question WHERE id = %d', intval($id));

  $question;
  while ($q = db_fetch_object($result)) {
    $question = $q;
  }
  krumo::dump($question->type);
    //根据 question 对象类型 (问答题:text/选择题:option),进行不同的页面跳转
  if ($question->type == "text") {
        //跳转至问答题页面
        //todo:将result的结果传递给 teacher_text_question_form 
    krumo::dump($question);
    $output = drupal_get_form('teacher_text_question_form',$question);
    
  } elseif ($question->type == "option") {
        //跳转至选择题页面
        //todo:
    $output = drupal_get_form('teacher_option_question_form',$question);
  } elseif ($question->type == "multiple_option") {

  } elseif ($question->type == "true_false") {
    $output = drupal_get_form('teacher_true_false_question_form',$question);
  } else {
    echo "123456";
  }
  return $output;
}
/**
 * [teacher_true_false_question_form 判断题录入]
 * @param  [type] $form_state [description]
 * @param  [type] $question   [description]
 * @return [type]             [description]
 */
function teacher_true_false_question_form(&$form_state,$question){
    //drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
    drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');
    $form = array(
        '#attributes' => array('class' => 'teacher-form'),
    );
    $form['type'] = array(
        '#type' => 'value',
        '#value' => "true_false"
    );
    //question id赋值
    $form['id'] = array(
        '#type' => 'value',
        '#value' => $question->id
    );
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
        '#required' => true,
        '#default_value' => $question->score,
    );
    //level
    $form['level'] = array(
        '#title' => '难度等级',
        '#type' => 'select',
        '#options' => array(1,2,3,4),
        '#required' => true,
        '#default_value' => $question->level,
    );
    //topic
    $form['topic'] = array(
        '#title' => '题目',
        '#type' => 'textarea',
        '#required' => true,
        '#default_value' => $question->topic,
    );

    //答案部分
    //$question->correct: 0:错误, 1:正确
    $form['radios'] = array(
        '#type' => 'radios',
        '#options' => array('错误','正确'),
        '#default_value' => $question->correct,
    );
    //
    //$form['#validate'] = array('add_option_question_validate');
    if ($question){
        //已有question调用update接口
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'submit',
            '#submit' => array('update_question_submit'),
        );
    }else{
        //新增question调用add接口
        $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'submit',
        '#submit' => array('add_question_submit'),
        );
    }
    return $form;
}

/**
 * 选择题试题录入
 */
function teacher_option_question_form(&$form_state,$question){
  drupal_add_css(drupal_get_path('module', 'teacher') .'/teacher.css', 'module', 'all', FALSE);
  drupal_add_js('sites/all/modules/teacher/teacher_add_text_question.js','file');
  
  $form = array(
    '#attributes' => array('class' => 'teacher-form'),
    );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => "option"
    );
  //question id赋值
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $question->id
    );

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
    '#required' => true,
    '#default_value' => $question->score,
    );

    //level
  $form['level'] = array(
    '#title' => '难度等级',
    '#type' => 'select',
    '#options' => array(1,2,3,4),
    '#required' => true,
    '#default_value' => $question->level,
    );

  $form['topic'] = array(
    '#title' => '题目',
    '#type' => 'textarea',
    '#required' => true,
    '#default_value' => $question->topic,
    );

    //选项 需要对question['option_a']进行反序列化操作
  $options = unserialize($question->option_a);

  $form['radios'] = array(
    '#type' => 'radios',
    '#options' => array('A','B','C','D'),
    '#default_value' => $question->correct,
    );

  $form['option_A'] = array(
    '#title' => '选择 A',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $options[0],
    );
  $form['option_B'] = array(
    '#title' => '选择 B',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $options[1],
    );
  $form['option_C'] = array(
    '#title' => '选择 C',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $options[2],
    );
  $form['option_D'] = array(
    '#title' => '选择 D',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $options[3],
    );

    //
  $form['#validate'] = array('add_option_question_validate');
  if ($question){
        //已有question调用update接口
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'submit',
      '#submit' => array('update_question_submit'),
      );
  }else{
        //新增question调用add接口
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'submit',
      '#submit' => array('add_question_submit'),
      );
  }
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

/**
 * 批量导入题库表单
 * @return [type] [description]
 */
function import_questions_form() {
  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['fieldset'] = array(
    '#type' => 'fieldset', 
    );
  $form['fieldset']['type'] = array(
    '#type' => 'select',
    '#title' => t('选择导入的试题类型'),
    '#default_value' => 'text',
    '#options' => array(
      'text' => t('问答题'), 
      'option' => t('单选题'), 
      'multiple_option' => t('多选题'),
      'true_false' => t('判断题'),
      ),
    );
  $form['fieldset']['upload'] = array(
    '#type' => 'file',
    '#title' => t('选择要导入的试题数据表格'),
    '#description' => t('文件必须是XLS格式'),
    '#size' => 40,
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('导入'),
    );

  $form['format_example'] = array(
    '#value' => l('下载题库格式示例', file_directory_path() . '/question_format_example.xls')
    );
  return $form;
}

/**
 * 从Excel批量导入试题
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function import_questions_form_submit($form, &$form_state) {
  require_once 'excel_reader2.php';
  $dest = file_directory_path() . '/upload';
  $file = file_save_upload('upload', array('file_validate_extensions' => array('xls')), $dest);
  if(!$file) {
    drupal_set_message('导入失败，请检查文件格式', 'error');
    return;
  }
  
  //检查文件是否可以按表格读取
  $data = null;
  try {
    $data = new Spreadsheet_Excel_Reader($file->filepath, false, 'UTF-8');
  } catch (Exception $e) {
    drupal_set_message('导入失败，请检查文件格式', 'error');
    return;
  }

  //按类型导入试题
  $result = null;
  switch ($data->val(1, 1)) {
    case 'option':
      $result = import_question_type_option($data);
      break;
    case 'multiple_option':
      $result = import_question_type_multiple_option($data);
      break;
    case 'true_false':
      $result = import_question_type_true_false($data);
      break;
    case 'text':
      $result = import_question_type_text($data);
      break;
    default:
      drupal_set_message('导入失败，请检查数据格式', 'error');
      return;
      break;
  }

  drupal_set_message('成功导入'. $result['success'] . '条试题！');
  if($fail > 0) {
    drupal_set_message($result['fail'] . '条试题导入失败，请检查数据格式！', 'error');
  }
  return;

}

/**
 * 导入单选题
 */
function import_question_type_option($data) {
  $rowcount = $data->rowcount();
  $row = 3;
  $success = 0;
  $fail = 0;
  while($row <= $rowcount) {
    $topic = $data->val($row, 3);
    $course = $data->val($row, 1);
    $chapter = $data->val($row, 2);
    $score =  2;
    $level = $data->val($row, 9);
    $correct = $data->val($row, 8);

    //序列化数组
    $a = $data->val($row, 4);
    $b = $data->val($row, 5);
    $c = $data->val($row, 6);
    $d = $data->val($row, 7);
    $teachersubject = array($a,$b,$c,$d);
    $con = serialize($teachersubject);

    //问题类型:
    $type = 'option';

    db_query("INSERT INTO {question} (id,topic,option_a,correct,course,type,chapter,score,level)
      VALUES(null,'%s','%s','%s','%s','%s','%s','%d','%d')",
      $topic,$con,$correct,$course,$type,$chapter,intval($score),intval($level));

    if(db_last_insert_id('question', 'id')) {
      $success += 1;
    }
    else {
      $fail += 1;
    }
    $row += 1;
  }

  $result = array(
    'success' => $success,
    'fail' => $fail,
    );
  return $result;
}

/**
 * 导入多选题
 */
function import_question_type_multiple_option($data) {
  $rowcount = $data->rowcount();
  $row = 3;
  $success = 0;
  $fail = 0;
  while($row <= $rowcount) {
    $topic = $data->val($row, 3);
    $course = $data->val($row, 1);
    $chapter = $data->val($row, 2);
    $score =  4;
    $level = $data->val($row, 13);
    $correct = $data->val($row, 12);

    //序列化数组
    $a = $data->val($row, 4);
    $b = $data->val($row, 5);
    $c = $data->val($row, 6);
    $d = $data->val($row, 7);
    $e = $data->val($row, 8);
    $f = $data->val($row, 9);
    $g = $data->val($row, 10);
    $h = $data->val($row, 11);
    $teachersubject = array($a, $b, $c, $d, $e, $f, $g, $h);
    $con = serialize($teachersubject);

    //问题类型
    $type = 'multiple_option';

    db_query("INSERT INTO {question} (id,topic,option_a,correct,course,type,chapter,score,level)
      VALUES(null,'%s','%s','%s','%s','%s','%s','%d','%d')",
      $topic,$con,$correct,$course,$type,$chapter,intval($score),intval($level));

    if(db_last_insert_id('question', 'id')) {
      $success += 1;
    }
    else {
      $fail += 1;
    }
    $row += 1;
  }

  $result = array(
    'success' => $success,
    'fail' => $fail,
    );
  return $result;
}

/**
 * 导入判断题
 */
function import_question_type_true_false($data) {
  $rowcount = $data->rowcount();
  $row = 3;
  $success = 0;
  $fail = 0;
  while($row <= $rowcount) {
    $topic = $data->val($row, 3);
    $course = $data->val($row, 1);
    $chapter = $data->val($row, 2);
    $score =  2;
    $level = $data->val($row, 5);
    $correct = $data->val($row, 4);

    //问题类型
    $type = 'true_false';

    db_query("INSERT INTO {question} (id,topic,correct,course,type,chapter,score,level)
      VALUES(null,'%s','%s','%s','%s','%s','%d','%d')",
      $topic,$correct,$course,$type,$chapter,intval($score),intval($level));

    if(db_last_insert_id('question', 'id')) {
      $success += 1;
    }
    else {
      $fail += 1;
    }
    $row += 1;
  }

  $result = array(
    'success' => $success,
    'fail' => $fail,
    );
  return $result;
}

/**
 * 导入问答题
 */
function import_question_type_text($data) {
  $rowcount = $data->rowcount();
  $row = 3;
  $success = 0;
  $fail = 0;
  while($row <= $rowcount) {
    $topic = $data->val($row, 3);
    $course = $data->val($row, 1);
    $chapter = $data->val($row, 2);
    $score =  10;
    $level = $data->val($row, 5);


    //问题类型:option/text
    $type = 'text';

    db_query("INSERT INTO {question} (id,topic,course,type,chapter,score,level)
      VALUES(null,'%s','%s','%s','%s','%d','%d')",
      $topic,$course,$type,$chapter,intval($score),intval($level));

    if(db_last_insert_id('question', 'id')) {
      $success += 1;
    }
    else {
      $fail += 1;
    }
    $row += 1;
  }

  $result = array(
    'success' => $success,
    'fail' => $fail,
    );
  return $result;
}
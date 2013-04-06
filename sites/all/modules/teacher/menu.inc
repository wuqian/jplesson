<?php
/**
 * Implementation of hook_menu().
 */
function teacher_menu() {
  $items['teacher'] = array(
    'title' => '教师平台',
    'description' => '发布通知、管理课程、管理作业、管理学生信息',
    'page callback' => 'teacher_task_list',
        'access arguments' => array('access content'), 
    'type' => MENU_NORMAL_ITEM,
  );
  $items['teacher/notices'] = array(
    'title' => '通知管理',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_notices'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/courses'] = array(
    'title' => '课程管理',
    'page callback' => 'teacher_admin_courses',
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/courses/edit'] = array(
    'title' => '修改课程分类',
    'page callback' => 'teacher_admin_courses_edit',
    'access arguments' => array('access teacher pages'),
    'type' => MENU_CALLBACK,
  );
  $items['teacher/courses/add'] = array(
    'title' => '添加课程分类',
    'page callback' => 'teacher_admin_courses_add',
    'access arguments' => array('access teacher pages'),
    'type' => MENU_CALLBACK,
  );
  $items['teacher/lessons/%'] = array(
    'title' => '课程管理',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_lessons', 2),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/tests'] = array(
    'title' => '作业管理',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_tests'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/test/%'] = array(
    'title' => '已提交作业列表',
    'page callback' => 'teacher_admin_test_list',
    'page arguments' => array(2),
    'access callback' => 'teacher_admin_test_list_access',
    'access arguments' => array(2), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/test/correct/%node'] = array(
    'title' => '已提交作业列表',
    'page callback' => 'node_page_edit',
    'page arguments' => array(3),
    'access callback' => 'teacher_test_correct_access', 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/students'] = array(
    'title' => '学生管理',
    'page callback' => 'teacher_students',
    'page arguments' => array('list'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/students/add'] = array(
    'title' => '添加学生用户',
    'page arguments' => array('add'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/students/import/user'] = array(
    'title' => '批量导入学生用户',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_import_user'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/students/import/cisco'] = array(
    'title' => '导入思科网院用户信息',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_import_cisco'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/news'] = array(
    'title' => '新闻管理',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('teacher_admin_news'),
    'access arguments' => array('access teacher pages'), 
    'type' => MENU_CALLBACK,
  );
  $items['teacher/lessons/new'] = array(
    'title' => '添加课程',
    'page callback' => 'node_add',
    'page arguments' => array('lesson'),
    'access callback' => 'node_access',
    'access arguments' => array('create', 'lesson'),
  );
  $items['teacher/news/add'] = array(
    'title' => '添加课程',
    'page callback' => 'node_add',
    'page arguments' => array('news'),
    'access callback' => 'node_access',
    'access arguments' => array('create', 'news'),
  );
   $items['teacher/notices/new'] = array(
    'title' => '添加课程',
    'page callback' => 'node_add',
    'page arguments' => array('notice'),
    'access callback' => 'node_access',
    'access arguments' => array('create', 'notice'),
   );
   $items['teacher/tests/new'] = array(
    'title' => '添加课程',
    'page callback' => 'node_add',
    'page arguments' => array('test'),
    'access callback' => 'node_access',
    'access arguments' => array('create', 'test'),
   );
   
    //试题管理
    $items['teacher/question/manage']=array(
        'title' => t('试题管理'),
        'page callback' => 'teacher_question_manage',
        'access arguments' => array('access teacher pages'),
        'type' => MENU_CALLBACK,
    );
    //试题录入
    $items['teacher/question/add/option']=array(
        'title' => t('选择题录入'),
        'page callback' => 'drupal_get_form',
        'page arguments' => array('teacher_option_question_form'),
        'access arguments' => array('access teacher pages'),
        'type' => MENU_CALLBACK,
    );
    $items['teacher/question/add/text']=array(
        'title' => t('问答题录入'),
        'page callback' => 'drupal_get_form',
        'page arguments' => array('teacher_text_question_form'),
        'access arguments' => array('access teacher pages'),
        'type' => MENU_CALLBACK,
    );
    //试题编辑
    $items['teacher/question/edit/%']=array(
      'title' => t('编辑题目'),
      'page callback' => 'question_edit',
      'page arguments' => array(3),
      'access arguments' => array('access teacher pages'),
      'type' => MENU_CALLBACK,
    );
    //试题删除
    $items['teacher/question/remove/%']=array(
      'title' => t('删除题目'),
      'page callback' => 'question_remove',
      'page arguments' => array(3),
      'access arguments' => array('access teacher pages'),
      'type' => MENU_CALLBACK,
    );

    //试卷管理-

    //手工添加试卷
    $items['teacher/questionpaper/add/manually'] = array(
        'title' => t('生成试卷'),
        'page callback' => 'teacher_add_manually_questionpaper',
        'access arguments' => array('access teacher pages'),
        'type' => MENU_CALLBACK,
    );

    //试卷查询与删除
    $items['teacher/questionpaper/manage'] = array(
        'title' => '试卷列表',
        // 'page callback' => 'teacher_question_paper',
        'page callback' => 'teacher_questionpaper_manage',
        'page arguments' => array('list'),
        'access arguments' => array('access teacher pages'),
        'type' => MENU_CALLBACK,
    );

    $items['teacher/topic/paper/%'] = array(
        'title' => '试卷信息',
        'page callback' => 'drupal_get_form',
        'page arguments' => array('teacher_question_list_all', 3),
        'access arguments' => array('access teacher pages'),
        'type' => MENU_NORMAL_ITEM,
    );
    $items['teacher/questionpaper/add/auto'] = array(
      'title' => '自动生成试卷',
      'access arguments' => array('access teacher pages'),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('teacher_add_auto_questionpaper'),
      'type' => MENU_NORMAL_ITEM,
    );
    $items['teacher/question/import'] = array(
      'title' => '批量导入试题',
      'access arguments' => array('access teacher pages'),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('import_questions'),
      'type' => MENU_NORMAL_ITEM,
    );
    $items['teacher/topic/paper/%/list'] = array(
        'title' => '试题',
        'access arguments' => array('The teacher input'),
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => 0,
    );
    $items['teacher/topic/paper/%/answer'] = array(
        'title' => '答题信息',
        'access arguments' => array('access teacher pages'),
        'page callback' => 'drupal_get_form',
        'page arguments' => array('teacher_overview_vocabularies', 3),
        'type' => MENU_LOCAL_TASK,
        'weight' => 1,
    );
  return $items;
}
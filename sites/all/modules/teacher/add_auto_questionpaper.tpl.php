<style type="text/css">
#class-option .form-checkbox{
    width: 100px;
    display: inline-block;
}

</style>
<form action="<?php echo url('teacher/questionpaper/add/auto') ?>"  accept-charset="UTF-8" method="post" id="teacher-add-auto-questionpaper" autocomplete='on'>
    <div class="form-item form-text required" id="edit-title-wrapper">
        <label for="edit-title">标题：
            <span class="form-required" title="此项必填。">*</span>
        </label>
        <input type="text" maxlength="128" name="title" id="edit-title" size="60" value="" class="form-text required">
    </div>
    <fieldset>
        <div class="form-radios">
            <div class="form-item form-radio">
                <label class="option" for="edit-paper_type-0">
                    <input type="radio" id="edit-paper_type-0" name="paper_type" value="考试题" checked="checked" class="form-radio"> 
                    考试题
                </label>
            </div>
            <div class="form-item form-radio">
                <label class="option" for="edit-paper_type-1">
                    <input type="radio" id="edit-paper_type-1" name="paper_type" value="练习题"  class="form-radio"> 
                    练习题
                </label>
            </div>
        </div>
    </fieldset>
    <fieldset>
        <label for="edit-title">答题时间(分钟)：
            <span class="form-required" title="此项必填。">*</span>
        </label>
        <input type="text" maxlength="128" name="timelimit" id="edit-timelimit" size="10" value="60" class="form-text required">
    </fieldset>
    <fieldset id="class-option">
        <legend>班级：</legend>
        <?php foreach ($params['classes'] as $key => $value): ?>
            <div class="form-item form-checkbox">
                <label class="option" for="option_class_id_<?php echo $key ?>">
                    <input type="checkbox" name="class_ids[]" value="<?php echo $value ?>" id="option_class_id_<?php echo $key ?>"> 
                    <?php echo $value ?>
                </label>
            </div>
        <?php endforeach ?>
    </fieldset>

    <fieldset>
        <legend>课程：</legend>
        <select name="course" class="form-select" id="edit-course">
            <?php foreach ($params['courses'] as $key => $value): ?>
                <option value="<?php echo $value['name'] ?>"><?php echo $value['name'] ?></option>
            <?php endforeach ?>
        </select>
    </fieldset>
    <fieldset>
        <label>试卷总计分数：</label>
        <span id="total-score">0</span>
        <span>分</span>
    </fieldset>
    <div id="settingsInput"></div>

    <input type="submit" name="op" id="edit-submit" value="生成试卷" class="form-submit">
</form>


<div id="unusedInput"  style="display:none;">

    <?php foreach ($params['courses'] as $key => $course): ?>
        <div class="course-settings" id="<?php echo $course['name'] ?>-setting">
            <fieldset class="types-settings">
                <legend>题型分布</legend>
                <?php foreach ($course['chapters'] as $key => $chapter): ?>
                    <div class="form-item">
                        <div>
                            <label><?php echo ($key + 1) . '. ' . $chapter ?></label>
                        </div>
                        <span class="field-prefix">单选题</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[option]" size="4" value="" class="form-text required">
                        <span class="field-prefix">多选题</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[multiple_option]" size="4" value="" class="form-text required">
                        <span class="field-prefix">判断题</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[true_false]" size="4" value="" class="form-text required">
                        <span class="field-prefix">问答题</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[text]" size="4" value="" class="form-text required text-type">
                    </div>
                <?php endforeach ?>
            </fieldset>
            <fieldset>
                <legend>难度分布</legend>
                <?php foreach ($course['chapters'] as $key => $chapter): ?>
                    <div class="form-item">
                        <div>
                            <label><?php echo ($key + 1) . '. ' . $chapter ?></label>
                        </div>
                        <span class="field-prefix">难度1</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[level1]" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度2</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[level2]" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度3</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[level3]" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度4</span> 
                        <input type="text" maxlength="2" name="chapter-<?php echo $key ?>[level4]" id="edit-option" size="4" value="" class="form-text required">
                    </div>
                <?php endforeach ?>
            </fieldset>
            <input type="hidden" name="chapterCount" value="<?php echo count($course['chapters']) ?>">
        </div>
        
    <?php endforeach ?>


</div>
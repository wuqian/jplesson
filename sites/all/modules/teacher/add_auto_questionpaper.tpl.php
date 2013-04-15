<form action="<?php echo url('teacher/questionpaper/add/auto') ?>"  accept-charset="UTF-8" method="post" id="teacher-add-auto-questionpaper">
    <div class="form-item form-text required" id="edit-title-wrapper">
        <label for="edit-title">标题：
            <span class="form-required" title="此项必填。">*</span>
        </label>
        <input type="text" maxlength="128" name="title" id="edit-title" size="60" value="" class="form-text required">
    </div>

    <div class="form-item form-select" id="edit-class-id-wrapper">
        <label for="edit-class-id">班级：</label>
        <select name="class_id" class="form-select" id="edit-class-id">
            <?php foreach ($params['classes'] as $key => $value): ?>
                <option value="<?php echo $value ?>"><?php echo $value ?></option>
            <?php endforeach ?>
        </select>
    </div>

    <div class="form-item form-select" id="edit-course-wrapper">
        <label for="edit-course">课程：</label>
        <select name="course" class="form-select" id="edit-course">
            <?php foreach ($params['courses'] as $key => $value): ?>
                <option value="<?php echo $value['name'] ?>"><?php echo $value['name'] ?></option>
            <?php endforeach ?>
        </select>
    </div>

    <div id="settingsInput"></div>

    <input type="submit" name="op" id="edit-submit" value="生成试卷" class="form-submit">
</form>


<div id="unusedInput"  style="display:none;">

    <?php foreach ($params['courses'] as $key => $course): ?>
        <div class="course-settings" id="<?php echo $course['name'] ?>-setting">
            <fieldset>
                <label>题型分布</label>
                <?php foreach ($course['chapters'] as $key => $chapter): ?>
                    <div class="form-item">
                        <div>
                            <label><?php echo ($key + 1) . '. ' . $chapter ?></label>
                        </div>
                        <span class="field-prefix">单选题</span> 
                        <input type="text" maxlength="2" name="option-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">多选题</span> 
                        <input type="text" maxlength="2" name="multiple_option-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">判断题</span> 
                        <input type="text" maxlength="2" name="true_false-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">问答题</span> 
                        <input type="text" maxlength="2" name="text-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                    </div>
                <?php endforeach ?>
            </fieldset>
            <fieldset>
                <label>难度分布</label>
                <?php foreach ($course['chapters'] as $key => $chapter): ?>
                    <div class="form-item">
                        <div>
                            <label><?php echo ($key + 1) . '. ' . $chapter ?></label>
                        </div>
                        <span class="field-prefix">难度1</span> 
                        <input type="text" maxlength="2" name="level1-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度2</span> 
                        <input type="text" maxlength="2" name="level2-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度3</span> 
                        <input type="text" maxlength="2" name="level3-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                        <span class="field-prefix">难度4</span> 
                        <input type="text" maxlength="2" name="level4-<?php echo $key ?>" id="edit-option" size="4" value="" class="form-text required">
                    </div>
                <?php endforeach ?>
            </fieldset>
            <input type="hidden" name="chapterCount" value="<?php echo count($course['chapters']) ?>">
        </div>
        
    <?php endforeach ?>


</div>
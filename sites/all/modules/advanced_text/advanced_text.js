
Drupal.behaviors.advancedText = function(context) {
  $('.advanced-text', context).hide();
  for (var field in Drupal.settings.advancedText){
    var advancedField = $('<div class="form-item advanced-text-wrapper"></div>');
    $.each(Drupal.settings.advancedText[field].field, function(i, piece) {
      if (typeof(piece['#value']) == 'undefined' ) {
        /* Text block */
        var input = $('<input type="text" />')
        .attr('maxlength', piece['#maxlength']).attr('size', piece['#size']);
        if (Drupal.settings.advancedText[field].options.jump) {
          input.focus(function() {
            jQuery.data(this, "prevVal", $(this).val());
          })
          .keyup(function(e) {
            if (input.val() != jQuery.data(this, "prevVal") && input.val().length >= input.attr('maxlength')) {
              input.nextAll('input:eq(0)').focus();
            }
            jQuery.data(this, "prevVal", $(this).val());
          });
        }
        if ($('#'+field).hasClass('error')) {
          input.addClass('error');
        }
        advancedField.append(input.get(0)).wrap(piece['#prefix']+piece['#suffix']);
      }
      else {
        /* Separator */
        advancedField.append(piece['#prefix']+piece['#value']+piece['#suffix']);
      }
    });
    /* Insert default value if field has no errors (don't know where to insert data if it is not valid) */
    var defVal = $('#'+field).val();
    if (defVal && !$('#'+field).hasClass('error')) {
      var pieces = advancedField.children();
      var pos = 0;
      var valArray = new Array;
      pieces.each(function() {
        if (len = $(this).attr('maxlength')) {
          valArray.push(defVal.substr(pos, len));
          pos += len;
        }
        else {
          valArray.push(defVal.substr(pos, $(this).text().length));
          pos += $(this).text().length;
        }
      });
      $.each(valArray, function(i) {
        maxLen = pieces.eq(i).attr('maxlength');
        if (pieces.eq(i) && maxLen && maxLen == valArray[i].length) {
          pieces.eq(i).val(valArray[i]);
        }
      });
    }
    $('#'+field).before(advancedField)
    .parents("form:eq(0)").bind('submit', field, function(e) {
      /* Apply value to hidden field on submit */
      var totVal = '';
      var childs = $('#'+e.data).siblings('.advanced-text-wrapper').children();
      childs.each(function() {
        $(this).is('input') ? totVal += $(this).val() : totVal += $(this).text();
      });
      totVal == childs.filter('.advanced-text-separator').text() ? $('#'+e.data).val('') : $('#'+e.data).val(totVal); 
    });
  };
};

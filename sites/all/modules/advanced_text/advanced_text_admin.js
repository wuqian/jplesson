Drupal.behaviors.advancedTextAdmin = function(context) {
  $('#edit-add-brick-submit', context).ajaxComplete(function() {
    // Remove the additional div added by ahah.js
    var wrapper = '#'+ Drupal.settings.ahah[$(this).attr('id')].wrapper;
    var contents = $(wrapper +' > div').contents();     
    $(wrapper+' > div').replaceWith(contents);
  });

  $('#advanced-text-table', context).ajaxStop(function(e) {
    // Reapply tabledrag settings
    $('a.tabledrag-handle', context).remove();
    $(this, context).removeClass('tabledrag-processed');
    delete Drupal.tableDrag['advanced-text-table'];
    Drupal.attachBehaviors(this);

    // TODO: Display status messages
  });

  // Disable js features when js is disabled
  $('#edit-javascript-options-js').click(function() {
    if ($(this).is(':not(:checked)')) {
      $('#edit-javascript-options-jump').attr('checked', false).attr('disabled', 'disabled');
    }
    else {
      $('#edit-javascript-options-jump').attr('disabled', false); 
    }
  });

  $('.allowed-values-input', context).hide();

  // Regular expression field
  $('.allowed-values-select', context).change(function() {
    if ($(this).val() == 'regexp') {
      $(this).parent().next().children().show();
    }
    else {
      $(this).parent().next().children().hide();
    }
  });
  $('.allowed-values-select', context).change();
};

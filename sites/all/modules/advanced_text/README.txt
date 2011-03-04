
ADVANCED TEXT
a Drupal module by Gregorio Magini


DESCRIPTION
Advanced Text provides a new widget for the "Text" field type. This new widget
is an enhanced version of the text widget, that can be divided in text blocks
and separators.
Each text block has its own options (allowed characters, length and size),
while separators can be given any value. The allowed characters of a text block
can be chosen from a few predefined options, or controlled with a custom
regular expression.
When editing a node, the Advanced Text field will be rendered with javascript
to match the chosen structure. Text blocks are input fields, and separators are
simple markup.


EXAMPLES
1) You can create a structured serial number field that only accepts certain
characters at given positions.
2) You can also assemble a "fill the gaps" field, that is presented to users
like this: "The quick brown ___ jumps over the lazy ___".


DEPENDENCIES
- CCK http://drupal.org/project/cck
- Text (included in CCK)


COMPATIBILITY
Advanced Text has been tested with Drupal 6.16 and CCK 2.6


INSTALLATION
- Navigate to admin/build/modules and enable the module.


USAGE
1) When creating a new CCK field, choose "Text" type and "Advanced text field"
  widget.

2) In the field editing page, you can easily add, rearrange and remove blocks
  and separators.

3) These are the options available in the field editing page:

- Options for text blocks:

    Allowed values.
    Select a type of allowed values. By default, the available
    options are: Any, Alphanumeric, Alphabetic, Numeric, Regular expression.
    If you select Regular expressions, a field will appear where you can input
    the expression. When the user submits a value, the validation of the form
    will fail if it doesn't match the expression. The string '{maxlength}' is
    a token that you can use in your expression to match the exact length of the
    field exactly one time.
    Examples:
    - The expression /^(dog|log|bog)$/ will validate only if the user enters 
    "dog", "log" or "bog".
    - The expression /^[C]{maxlength}$/ will validate only if the user enters
    the letter "C" exactly the "maxlength" value of the text block.
    Note: Only users with the permission "Use PHP input for field settings
    (dangerous - grant with care)" can edit the Regexp option.

    Exact length.
    The submitted value of the block must be no more and no less
    than this option. In the future we will enable also an option to set only a
    minimum and/or a maximum length.

    Size.
    The "size" attribute of the text block.

- Options for separators:

    Value.
    The markup presented to the user between the preceding and 
    following block.

- Description options:
  A description of the required format is created
  automatically by the module and is presented below the Help text on the node
  editing form. Here you can choose the format of this description:

    Display separators as underscores.
    The description of the example #2 would be something like this:
    _***_***, with a legend explaining the symbols.

    Display full separators.
    Example #2 description becomes: The quick brown *** jumps over the lazy ***.

    Do not display description (only Help text).

- Javascript options:

    Use javascript to enhance the widget.
    You can disable this option if you want the field to be presented to the 
    user as a regular text field.

    Jump to next block when current is filled out.
    When enabled, if a user fills out a text block, the caret will jump
    automatically to the next block.


DEVELOPER INFORMATION
- Custom regular expressions:
  
  Developers can add new regular expression types by adding a function to their
  module like this:
  
    function MYMODULE_advanced_text_custom_types($types) {
      $custom_types = array(
        'example' => array(
          'stub' => '?',
          'description' => t('Example custom type'),
          'regexp' => '/^(yes|no)$/', // Allow only the strings "yes" and "no"
                                      // from user input
        ),
      );
      return $custom_types;
    }
  
  The $types variable contains the predefined types, that can be modified but 
  not removed.

- Theming:
  The module introduces two theme functions: theme_advanced_text and
  theme_advanced_text_description.


MAINTENANCE
The Advanced Text module is developed and maintained by:
Gregorio Magini http://drupal.org/user/55674

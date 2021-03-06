<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <title><?php print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <script src="<?php echo $base_path; ?>misc/jquery-migrate-1.2.1.min.js" type="text/javascript"></script>
    <script src="<?php echo $base_path; ?>misc/liHover.js" type="text/javascript"></script>
    <!--[if lt IE 7]>
      <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
  </head>
  <body<?php print phptemplate_body_class($left, $right); ?>>

<!-- Layout -->
  <div id="wrapper">
    <div id="container" class="clear-block">
      
      <div id="header">
        <?php if ($left): ?>
          <div id="sidebar-left" class="sidebar">
            <?php print $left ?>
          </div>
        <?php endif; ?>
      </div> <!-- /header -->
      <div id="nav" class="clear-block"><?php print $header; ?></div>
      <div id="center"><div id="squeeze">
          <?php if ($mission): print '<div id="mission">'. $mission .'</div>'; endif; ?>
          <?php if ($tabs): print '<div id="tabs-wrapper" class="clear-block">'; endif; ?>
          <?php if ($title): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>
          <?php if ($tabs): print '<ul class="tabs primary">'. $tabs .'</ul></div>'; endif; ?>
          <?php if ($tabs2): print '<ul class="tabs secondary">'. $tabs2 .'</ul>'; endif; ?>
          <?php if ($show_messages && $messages): print $messages; endif; ?>
          <?php print $help; ?>
          <div class="clear-block">
	          <?php if ($before_content): ?>
	          	<div id="before-content">
	            	<?php print $before_content ?>
	          	</div>
	        	<?php endif; ?>
            <?php print $content ?>
          </div>
          <div id="footer"><?php print $footer_message . $footer ?></div>
          </div></div> <!-- /.left-corner, /.right-corner, /#squeeze, /#center -->

      <?php if ($right): ?>
        <div id="sidebar-right" class="sidebar">
          <?php if (!$left && $search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
          <?php print $right ?>
        </div>
      <?php endif; ?>

    </div> <!-- /container -->
  </div>
<!-- /layout -->
  <script type="text/javascript">
    var navCount = 0;
    $(document).ready(function(){
      $("#nav .content").children(".menu").children("li").each(function() {
        $(this).css("background-position",navCount);
        navCount = navCount - 110;
      });
    });
  </script>
  <?php print $closure ?>
  </body>
</html>

<?php
// $Id$

/**
 * @file
 * Privatemsg thread (a list of messages) template.
 * 
 * Available variables:
 * - $thread: array with thread properties
 * - $participants: themed participant list
 * - $thread_view: themed thread view
 * - $reply_form: themed reply form
 */
?>

<div>
<span><?php print t('Participants') .':'; ?></span>
<span><?php print $participants; ?></span>
</div>
<?php print $thread_view; ?>
<?php print $reply_form; ?>
<?php
// $Id$

/**
 * @file
 * Handler for the "participants" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_participants extends views_handler_field_prerender_list {
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_user'] = array('default' => TRUE);
    $options['replace_current'] = array('default' => '');    
    return $options;
  }

  /**
   * Provide "link to profile" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_user'] = array(
      '#title' => t('Link usernames to profiles'),
      '#description' => t('When enabled, it will show usernames as links to user profiles'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['link_to_user'],
    );
    $form['replace_current'] = array(
      '#type' => 'textfield',
      '#title' => t('Replace current user with'),
      '#description' => t('If provided, current user name in the list will be replaced with this string. Possible use cases: "You", "Me".'),
      '#default_value' => $this->options['replace_current'],
    );
  }

  /**
   * Load participant data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    global $user;
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $thread_id = $value->{$this->field_alias};
        $result = db_query("SELECT DISTINCT(pmi.uid), u.name FROM {pm_index} pmi LEFT JOIN {users} u ON pmi.uid = u.uid WHERE pmi.thread_id = %d", $thread_id);
        while ($thread = db_fetch_object($result)) {
          $uid = $thread->uid;
          $username = $thread->name;
          
          if ($user->uid == $uid && !empty($this->options['replace_current'])) {
            $replace = t($this->options['replace_current']);
            $this->items[$thread_id][$uid]['name'] = filter_xss_admin($replace);
          }
          elseif (!empty($this->options['link_to_user']) && user_access('access user profiles')) {
            $account = user_load(array('uid' => $uid));
            $this->items[$thread_id][$uid]['name'] = theme('username', $account);
          }
          else {
            $this->items[$thread_id][$uid]['name'] = check_plain($username);
          }
          
          $this->items[$thread_id][$uid]['thread_id'] = $thread_id;
        }
      }
    }
  }

  function render_item($count, $item) {
    return $item['name'];
  }
  
  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-thread-id' . ']'] = t('The thread ID.');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-thread-id' . ']'] = $item['thread_id'];
  }  
  
  
  
}

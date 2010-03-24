<?php
// $Id$

/**
 * @file
 * Handler for the "message count" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_msg_count extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['uid'] = array('table' => 'pm_index', 'field' => 'uid');
  }
  
  function option_definition() {
    $options = parent::option_definition();
    $options['count_filter'] = array('default' => 'not_deleted');    
    return $options;
  }

  function filtering_options() {
    $options = array(
      'not_deleted' => t('Not deleted messages'),      
      'read' => t('Read and not deleted messages'),
      'unread' => t('Unread and not deleted messages'),
      'deleted' => t('Deleted messages'),
      'all' => t('All messages'),
    );
    return $options;
  }
  
  /**
   * Provide "link to profile" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['count_filter'] = array(
      '#title' => t('Count'),
      '#description' => t('Please select which messages to count.'),
      '#type' => 'radios',
      '#options' => $this->filtering_options(),
      '#default_value' => $this->options['count_filter'],
    );
  }
  
  /**
   * Load message count data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    // Prepare thread list for IN list.
    $threads = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $threads[] = $value->{$this->field_alias};
      }
    }
    $query  = "SELECT pmi.thread_id, pmi.uid, COUNT(pmi.mid) AS msg_count FROM {pm_index} pmi";
    $query .= " WHERE pmi.thread_id IN (" . db_placeholders($threads) . ")";
    switch ($this->options['count_filter']) {
      case 'not_deleted':
        $query .= " AND pmi.deleted = 0";
        break;      
      case 'read':
        $query .= " AND pmi.is_new = 0 AND pmi.deleted = 0";
        break;
      case 'unread':
        $query .= " AND pmi.is_new = 1 AND pmi.deleted = 0";
        break;
      case 'deleted':
        $query .= " AND pmi.deleted > 0";
        break;
      case 'all':
        // no filtering
    }
    $query .= " GROUP BY pmi.thread_id, pmi.uid";
    $result = db_query($query, $threads);
    while ($thread = db_fetch_object($result)) {
      // Add uid key so every participant gets own msg count.
      $this->items[$thread->thread_id][$thread->uid][0]['msg_count'] = $thread->msg_count;
      $this->items[$thread->thread_id][$thread->uid][0]['msg_count_raw'] = $thread->msg_count;
    }
  }
  
  function get_items($values) {
    $uid = $values->{$this->aliases['uid']};
    $thread_id = $values->{$this->field_alias};
    if (!empty($this->items[$thread_id][$uid])) {
      return $this->items[$thread_id][$uid];
    }
    return array(0 => array('msg_count' => 0));
  }
  
  function render_item($count, $item) {
    return $item['msg_count'];
  }
  
  function document_self_tokens(&$tokens) {
    $options = $this->filtering_options();
    $id = $this->options['id'];
    $prefix = $options[$this->options['count_filter']];

    $tokens['[' . $id . '-raw' . ']'] = $prefix . ': ' . t('raw number of messages.');
    $tokens['[' . $id . '-yes-no' . ']'] = $prefix . ': ' . t('"Yes" or "No" depending on if number is > 0 or 0.');
    $tokens['[' . $id . '-string' . ']'] = $prefix . ': ' . t('number as a string e.g. "5 messages".');
    
    switch($this->options['count_filter']) {
      case 'unread':
        $tokens['[' . $id . '-new-empty' . ']'] = $prefix . ': ' . t('"#new" or empty string depending on if number is > 0 or 0. Useful for anchor fragments.');
        $tokens['[' . $id . '-string' . ']'] = $prefix . ': ' . t('number as a string e.g. "5 new messages"');
        break;
        
      case 'deleted':
        $tokens['[' . $id . '-string' . ']'] = $prefix . ': ' . t('number as a string e.g. "5 deleted messages"');
        break;
    }
  }

  function add_self_tokens(&$tokens, $item) {
    $count = $item['msg_count_raw'];
    $id = $this->options['id'];
    
    $tokens['[' . $id . '-raw' . ']'] = $count > 0 ? $count : "0";
    $tokens['[' . $id . '-yes-no' . ']'] = $count > 0 ? t('Yes') : t('No');
    $tokens['[' . $id . '-string' . ']'] = $count > 0 ? format_plural($count, '1 message', '@count messages', array('@count' => $count))  : '';
    
    switch($this->options['count_filter']) {
      case 'unread':
        $tokens['[' . $id . '-new-empty' . ']'] = $count > 0 ? '#new' : '';
        $tokens['[' . $id . '-string' . ']'] = $count > 0 ? format_plural($count, '1 new message', '@count new messages', array('@count' => $count)) : ''; 
        break;

      case 'deleted':
        $tokens['[' . $id . '-string' . ']'] = $count > 0 ? format_plural($count, '1 deleted message', '@count deleted messages', array('@count' => $count))  : '';
        break;
    }
  }
  
}

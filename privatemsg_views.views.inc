<?php
// $Id$
/**
 * @file
 * Privatemsg Views module Views hooks.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function privatemsg_views_views_data_alter(&$data) {  
  // ----------------------------------------------------------------
  // privatemsg table -- basic table information.
  $data['pm_message']['table']['group']  = t('Privatemsg');
  // Advertise this table as a possible base table
  $data['pm_message']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Private message'),
    'help' => t("Private messages sent between users."),
    'weight' => 10,
  );
  $data['pm_message']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'author',
    ),
  );
  
  // ----------------------------------------------------------------
  // pm_message table -- fields

  // mid
  $data['pm_message']['mid'] = array(
    'title' => t('Mesage Id'),
    'help' => t('The ID of the private message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
  );
  
  // Title
  $data['pm_message']['subject'] = array(
    'title' => t('Subject'),
    'help' => t('The subject of the message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Body field
  $data['pm_message']['body'] = array(
    'title' => t('Body'), // The item it appears as on the UI,
    'help' => t('The body of the message.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'format' => variable_get('filter_default_format', 1), // The name of the format field or an integer
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // Author field
  $data['pm_message']['author'] = array(
    'title' => t('Author'),
    'help' => t('Author of the message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
  );

  // Created field
  $data['pm_message']['timestamp'] = array(
    'title' => t('Sent date'),
    'help' => t('The date and time when the message was sent.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  
  // ----------------------------------------------------------------------
  // privatemsg message instance table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['pm_index']['table']['group']  = t('Privatemsg');

  // For other base tables, explain how we join
  $data['pm_index']['table']['join'] = array(
    // Directly link to privatemsg table.
    'pm_message' => array(
      'left_field' => 'mid',
      'field' => 'mid',
      'type' => 'INNER',
    ),
  );

  // mid
  $data['pm_index']['thread_id'] = array(
    'title' => t('Message thread'),
    'help' => t('Messages thread ID.'),
    // Information for displaying the nid
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a mid as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
  );

  // uid
  $data['pm_index']['uid'] = array(
    'title' => t('Participant'),
    'help' => t('Show messages where a user is either an author or a recipient.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'participant', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
  );
  
  // "New" marker
  $data['pm_index']['is_new'] = array(
    'title' => t('New ?'),
    'help' => t('Whether the user has read this message.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
  );

  // "Deleted" internal flag.
  $data['pm_index']['deleted'] = array(
    'title' => t('Deleted ?'),
    'help' => t('Whether the user has deleted this message.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Deleted?'),
      'type' => 'yes-no',
    ),
  );

  // Pseudo-field to show participants.
  $data['pm_index']['participants'] = array(
    'title' => t('Participants'),
    'help' => t('Participants.'),
    'real field' => 'thread_id',
    'field' => array(
      'title' => t('Participants'),
      'help' => t('Display user participants.'),
      'handler' => 'privatemsg_views_handler_field_participants',
      'skip base' => 'pm_index',
    ),
  );
  
  // "number of messages in a thread" pseudo-field.
  $data['pm_index']['count_messages'] = array(
    'title' => t('Message count'),
    'help' => t('Number of messages in the thread.'),
    'real field' => 'thread_id',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_msg_count',
    ),
  );

  // ----------------------------------------------------------------------
  // Pseudo tables to emulate aggregate queries.
  
  // First message.
  $data['pmi2']['table']['group']  = t('Privatemsg');
  $data['pmi2']['table']['join'] = array(
    'pm_message' => array(
      'table' => 'pm_index',
      'left_table' => 'pm_index',
      'left_field' => 'uid',
      'field' => 'uid',
      'extra' => 'pm_index.thread_id = pmi2.thread_id AND pm_index.mid > pmi2.mid',
    ),
  );
  $data['pmi2']['thread_id'] = array(
    'title' => t('first message in a thread'),
    'help' => t('Turns private message list into list of first messages.'),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_is_null',
    ),
  );

  // Last message.
  $data['pmi3']['table']['group']  = t('Privatemsg');
  $data['pmi3']['table']['join'] = array(
    'pm_message' => array(
      'table' => 'pm_index',
      'left_table' => 'pm_index',
      'left_field' => 'uid',
      'field' => 'uid',
      'extra' => 'pm_index.thread_id = pmi3.thread_id AND pm_index.mid < pmi3.mid',
    ),
  );
  $data['pmi3']['thread_id'] = array(
    'title' => t('last message in a thread'),
    'help' => t('Turns private message list into list of last messages.'),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_is_null',
      //'allow empty' => TRUE,
    ),
  );
    
  // ----------------------------------------------------------------------
  // Join users table to ours
  $data['users']['table']['join']['pm_message'] = array(
    'left_field' => 'author',
    'field' => 'uid',
    'type' => 'INNER',  
  );
  
}

/**
 * Implementation of hook_views_handlers().
 */
function privatemsg_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'privatemsg_views') . '/handlers',
    ),
    'handlers' => array(
      'privatemsg_views_handler_filter_is_null' => array(
        'parent' => 'views_handler_filter',
      ), 
      'privatemsg_views_handler_field_msg_count' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'privatemsg_views_handler_field_participants' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
    ),
  );
}



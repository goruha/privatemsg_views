<?php
// $Id$

/**
 * Simple "IS NULL" filter serving as helper for emulating aggregate queries
 * with joins. 
 */
class privatemsg_views_handler_filter_is_null extends views_handler_filter {
  function query() {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.$this->real_field IS NULL");
  }
  /**
   * Remove lonely "=" from admin summary.
   */
  function admin_summary() {
    return '';
  }
}

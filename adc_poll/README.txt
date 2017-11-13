ADC POLL
--------

We created a small Drupal module for enabling anonymous users to vote more than once in a poll.

We extended the core poll module by adding a custom submit function for the poll_view_voting form: https://api.drupal.org/api/drupal/modules%21poll%21poll.module/function/poll_view_voting/7.x

That function (named as _adc_poll_update_ip) get the last record from the poll_vote table, get the IP field from that record (which is unique by default, that's the constraint from the table definition), concatenates the timestamp to that IP (so that IP field will be unique, even if the user votes from the same IP many times) and updates that record.

You can see the module in action on http://34.193.52.202/adc, on the bottom of the page there is a poll in which the anon user can vote many times.

This could be a possible solution for this issue: https://www.drupal.org/forum/support/post-installation/2011-01-05/vote-for-anonymous-poll-module
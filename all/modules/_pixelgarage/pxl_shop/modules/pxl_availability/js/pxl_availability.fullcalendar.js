/**
 * THis file contains the fullcalendar option enhancements.
 *
 * Created by ralph on 24.01.17.
 */

(function ($) {

  Drupal.fullcalendar.plugins.pxl_availability = {
    options: function (fullcalendar, settings) {
      /**
       * Returns a date range with startDate < endDate and the times set.
       */
      var _harmonizeRange = function(startDateVal, endDateVal) {
        //
        // switch dates, if necessary
        var start, end;
        start = (startDateVal > endDateVal) ? endDateVal : startDateVal;
        end = (startDateVal > endDateVal) ? startDateVal : endDateVal;

        //
        // set start and end time for date range
        // add 15 hours to start date (check-in time)
        start += 15*3600*1000;
        // add 12 hours to end date (check-out time)
        end += 12*3600*1000;

        return [start, end];
      };

      /**
       * Parse Calendar events from the DOM.
       */
      var _parseCalendarEvents = function () {
        var events = [];
        var details = fullcalendar.$calendar.find('.fullcalendar-event-details');
        for (var i = 0; i < details.length; i++) {
          var event = $(details[i]),
            start = Date.parse(event.attr('start')),
            end = Date.parse(event.attr('end'));
          events.push({startTime: start, endTime: end});
        }
        return events;
      };

      /**
       * Validates a given date range against all calendar events.
       * Returns TRUE, if the given range does not intersect with any calendar event and lies in the future, FALSE otherwise.
       */
      var _dateRangeIsValid = function(start, end) {
        // get error element in dialog and initialize it
        var $errLabel = Drupal.settings.pxl_availability.modalCalendarDialog.find('.modal-header > .label');
        $errLabel.html('');

        // check if date range is in future
        if (start < Date.now()) {
          $errLabel.html(Drupal.settings.pxl_availability.error_in_past);
          return false;
        }

        // check against all calendar events
        var calendarEvents = _parseCalendarEvents();
        for (var i = 0; i < calendarEvents.length; i++) {
          var calendarEvent = calendarEvents[i];
          if (start <= calendarEvent.endTime && end >= calendarEvent.startTime) {
            $errLabel.html(Drupal.settings.pxl_availability.error_no_availability);
            return false;
          }
        }

        // no intersection
        return true;
      };

      /**
       * A form field click opens the fullcalendar always without any dates set. After a valid date range selection
       * in the fullcalendar (two single day clicks or a dragged selection), the dialog is closed and the form fields
       * are filled with the valid formatted dates.
       */
      return {
        selectable: true,

        select: function( startDate, endDate, allDay, jsEvent, view ) {
          // handle only ranges
          var startDateVal = startDate.getTime(),
            endDateVal = endDate.getTime();
          if (startDateVal === endDateVal) return;

          //
          // get correct range
          var $startFormField = Drupal.settings.pxl_availability.startFormField,
            $endFormField = Drupal.settings.pxl_availability.endFormField,
            $submitButton = Drupal.settings.pxl_availability.submitButton,
            range = _harmonizeRange(startDateVal, endDateVal);
          startDateVal = range[0];
          endDateVal = range[1];

          //
          // validate range
          var startDate = new Date(startDateVal),
            endDate = new Date(endDateVal);

          if (_dateRangeIsValid(startDateVal, endDateVal)) {
            $startFormField.val(startDate.toLocaleString('de-CH'));
            $startFormField.attr('data-time', startDateVal);
            $endFormField.val(endDate.toLocaleString('de-CH'));
            $endFormField.attr('data-time', endDateVal);

            //
            // close modal calendar dialog and enable submit button
            Drupal.settings.pxl_availability.modalCalendarDialog.modal('hide');
            $submitButton.attr('disabled', false);
          }

        },

        dayClick: function(date, allDay, jsEvent, view) {
          //
          // The day selection is validated, meaning a selection is only accepted, if it is not intersecting
          // with any of the existing calendar events.
          var $clickedFormField = Drupal.settings.pxl_availability.clickedFormField,
            $startFormField = Drupal.settings.pxl_availability.startFormField,
            $endFormField = Drupal.settings.pxl_availability.endFormField,
            $submitButton = Drupal.settings.pxl_availability.submitButton,
            clickedFormFieldIsStartField = $clickedFormField && $clickedFormField.attr('id').indexOf('start-date') >= 0,
            isSecondDateSelection = clickedFormFieldIsStartField ?
              $startFormField.val().length > 0 :
              $endFormField.val().length > 0,
            startDateVal = date.getTime(),
            endDateVal = startDateVal;

          if (isSecondDateSelection) {
            //
            // second date click in calendar, correct and validate date range
            // and close modal calendar dialog, if validation succeeds
            if (clickedFormFieldIsStartField) {
              startDateVal = parseInt($startFormField.attr('data-time'));
            }
            else {
              endDateVal = parseInt($endFormField.attr('data-time'));
            }

            // get correct range
            var range = _harmonizeRange(startDateVal, endDateVal);
            startDateVal = range[0];
            endDateVal = range[1];

          }

          //
          // validate date range
          var startDate = new Date(startDateVal),
            endDate = new Date(endDateVal);

          if (_dateRangeIsValid(startDateVal, endDateVal)) {
            //
            // set date range on fields
            if (isSecondDateSelection) {
              $startFormField.val(startDate.toLocaleString('de-CH'));
              $startFormField.attr('data-time', startDateVal);
              $endFormField.val(endDate.toLocaleString('de-CH'));
              $endFormField.attr('data-time', endDateVal);

              //
              // close modal calendar dialog and enable submit button
              Drupal.settings.pxl_availability.modalCalendarDialog.modal('hide');
              $submitButton.attr('disabled', false);
            }
            else if (clickedFormFieldIsStartField) {
              $startFormField.val(startDate.toLocaleString('de-CH'));
              $startFormField.attr('data-time', startDateVal);
            }
            else {
              $endFormField.val(startDate.toLocaleString('de-CH'));
              $endFormField.attr('data-time', startDateVal);
            }

            // change the selected day background color
            //$(this).css('background-color', 'MediumTurquoise');
            return true;
          }

          return false;
        }
      };
    }
  };

})(jQuery);


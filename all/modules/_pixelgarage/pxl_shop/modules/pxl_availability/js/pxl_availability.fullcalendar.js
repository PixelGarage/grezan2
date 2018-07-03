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
        // add check-in time in milliseconds
        start += Drupal.settings.pxl_availability.checkInTime*3600*1000;
        // add check-out time  in milliseconds
        end += Drupal.settings.pxl_availability.checkOutTime*3600*1000;

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
       * Returns TRUE, if the given range does not intersect with any calendar event, has a minimum number of days
       * and lies in the future, FALSE otherwise.
       */
      var _dateRangeIsValid = function(start, end) {
        // get error element in dialog and initialize it
        var $errLabel = Drupal.settings.pxl_availability.modalCalendarDialog.find('.modal-header > .label');
        $errLabel.html('');

        // check if date range is in future
        if (start < Date.now()) {
          $errLabel.html(Drupal.settings.pxl_availability.errorInPast);
          return false;
        }

        // check if date range has minimum number of days
        if (start != end) {
          $days = Math.round((end - start) / (24*3600*1000));
          if ($days < Drupal.settings.pxl_availability.minDays) {
            $errLabel.html(Drupal.settings.pxl_availability.errorMinDays);
            return false;
          }
        }

        // check against all calendar events
        var calendarEvents = _parseCalendarEvents();
        for (var i = 0; i < calendarEvents.length; i++) {
          var calendarEvent = calendarEvents[i];
          if (start <= calendarEvent.endTime && end >= calendarEvent.startTime) {
            $errLabel.html(Drupal.settings.pxl_availability.errorNoAvailability);
            return false;
          }
        }

        // valid date range
        return true;
      };

      /**
       * A form field click opens the fullcalendar always without any dates set. After a valid date range selection
       * in the fullcalendar (two single day clicks or a dragged selection), the dialog is closed and the form fields
       * are filled with the valid formatted dates.
       */
      var locale = 'de-CH',
        hiddenDays = ('pxl_availability' in Drupal.settings) ? Drupal.settings.pxl_availability.hiddenDays : [];

      return {
        selectable: true,

        hiddenDays: hiddenDays,

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

          //
          // validate datetime range
          startDateVal = range[0];
          endDateVal = range[1];
          startDate = new Date(startDateVal);
          endDate = new Date(endDateVal);

          if (_dateRangeIsValid(startDateVal, endDateVal)) {
            $startFormField.val(startDate.toLocaleString(locale));
            $startFormField.attr('data-time', startDateVal);
            $endFormField.val(endDate.toLocaleString(locale));
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
          if (_dateRangeIsValid(startDateVal, endDateVal)) {
            var startDate = new Date(startDateVal),
              endDate = new Date(endDateVal);

            //
            // set date range on fields
            if (isSecondDateSelection) {
              $startFormField.val(startDate.toLocaleString(locale));
              $startFormField.attr('data-time', startDateVal);
              $endFormField.val(endDate.toLocaleString(locale));
              $endFormField.attr('data-time', endDateVal);

              //
              // close modal calendar dialog and enable submit button
              Drupal.settings.pxl_availability.modalCalendarDialog.modal('hide');
              $submitButton.attr('disabled', false);
            }
            else if (clickedFormFieldIsStartField) {
              $startFormField.val(startDate.toLocaleString(locale));
              $startFormField.attr('data-time', startDateVal);
            }
            else {
              $endFormField.val(startDate.toLocaleString(locale));
              $endFormField.attr('data-time', startDateVal);
            }

            // change the selected day background color
            //$(this).css('background-color', 'MediumTurquoise');
            return true;
          }

          return false;
        },

        eventClick: function(calEvent, jsEvent, view) {
          //
          // prevent event click
          return false;
        }
      };
    }
  };

})(jQuery);


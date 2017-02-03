/**
 * Created by ralph on 24.01.17.
 */

(function ($) {

  /**
   * Opens the calendar popup on form element click.
   */
  Drupal.behaviors.availabilityFormClicks = {
    attach: function (context) {
      // Iterate through all availability block instances
      $.each(Drupal.settings.pxl_availability.blocks, function (block_id, settings) {
        var $block = $('#' + block_id),
          $modal = $block.find('.modal'),
          $formFields = $block.find('#pxl-availability-check-form .form-text'),
          $submitButton = $block.find('#pxl-availability-check-form .form-submit'),
          $errorLabel = $modal.find('.modal-header > .label');

        //
        // form field clicks
        $formFields.once('field-click', function () {
          $(this).on('click', function () {
            //
            // store clicked form field and start and end form field
            var clickedFormFieldIsStartField = $(this).attr('id').indexOf('start-date') >= 0,
              $otherField;

            //
            // set clicked field, start and end field and submit button in Drupal settings
            // to be accessible for fullcalendar
            Drupal.settings.pxl_availability.clickedFormField = $(this);
            $otherField = ($formFields[0] == this) ? $($formFields[1]) : $($formFields[0]);
            Drupal.settings.pxl_availability.submitButton = $submitButton;

            if (clickedFormFieldIsStartField) {
              Drupal.settings.pxl_availability.startFormField = $(this);
              Drupal.settings.pxl_availability.endFormField = $otherField;
            }
            else {
              Drupal.settings.pxl_availability.startFormField = $otherField;
              Drupal.settings.pxl_availability.endFormField = $(this);
            }

            //
            // reset form fields, error label and disable submit button
            $(this).val('');
            $(this).removeAttr('data-time');
            $otherField.val('');
            $otherField.removeAttr('data-time');
            $errorLabel.html('');
            $submitButton.attr('disabled', true);

            //
            // open calendar
            Drupal.settings.pxl_availability.modalCalendarDialog = $modal;
            Drupal.settings.pxl_availability.checkInTime = settings.check_in_time;
            Drupal.settings.pxl_availability.checkOutTime = settings.check_out_time;
            Drupal.settings.pxl_availability.hiddenDays = settings.hidden_days;
            $modal.fadeIn(300).modal('show');

            // Trigger a window resize so that calendar will redraw itself.
            $(window).resize();
          });
        });

        //
        // submit button click
        $submitButton.attr('disabled', true);
        $submitButton.once('button-click', function () {
          $(this).on('click', function (e) {
            var $blockContainer = $block.parent(),
              startVal = Drupal.settings.pxl_availability.startFormField.attr('data-time') / 1000, // unix timestamp in seconds
              endVal = Drupal.settings.pxl_availability.endFormField.attr('data-time') / 1000,
              params = {
                startDateVal: startVal,
                endDateVal: endVal,
                availableSKUs: settings.SKUs
              };

            //
            // load shopping cart with stripe payment (replace entire availability form)
            $blockContainer.load('/ajax/availability/submit', params, function (response, status, xhr) {
              if (status == "error") {
                var msg = "Server error " + xhr.status + ": " + xhr.statusText;
                $errorLabel.html(msg);
              }
              else {
                // attach behaviours to new block content
                Drupal.attachBehaviors($blockContainer);
              }
            });

            e.preventDefault();
          });
        });
      });
    }
  };


})(jQuery);

